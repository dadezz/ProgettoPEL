#include "json.hpp"

using std::string;
using std::pair;
using std::endl;

struct json::impl {
    enum tipologia {null, num, str, cond, lis, diz};
    tipologia Tipo;
    double numero;
    bool condizione;
    string stringa;
    struct Lista {
        json info;
        Lista* next;
    };
    struct Dizionario {
        pair<string, json> info;
        Dizionario* next;
    };

    typedef Lista* List;
    typedef Dizionario* Dict;

    // puntatori a fine e inizio lista/dizionario
    List list_head, list_tail;
    Dict dict_head, dict_tail;

    void delete_everything();   // riporta un qualsiasi json a tipo "null". ma non dealloca pimpl
    void copy(json const & rhs);    // su un json di tipo null, copia (deep) lo stato di rhs su this
    void cancella_list();  // dealloca la lista e setta a nullptr i puntatori
    void cancella_dict();  // dealloca la lista e setta a nullptr i puntatori
    void copia_list(List const & rhs);// copio una lista da un oggetto a this (vuoto)
    void copia_dict(Dict const & rhs);

};
// default constructor = sets json obj to "null" type
json::json(){
    pimpl = new impl;
    pimpl->Tipo = pimpl->null;
    pimpl->numero = 0;
    pimpl->stringa = "";
    pimpl->condizione = false;
    pimpl->list_head = pimpl->list_tail = nullptr;
    pimpl->dict_head = pimpl->dict_tail = nullptr;
}
// copy constructor
json::json(json const& rhs){
    *this = rhs;
}
// copy assignment
json& json::operator=(json const& rhs){
    if (this != &rhs){
        pimpl->delete_everything();
        pimpl->copy(rhs);
    }
    return *this;
}
// riporta un qualsiasi json a tipo "null". ma non dealloca pimpl
void json::impl::delete_everything(){
    Tipo = null;
    numero = 0;
    stringa = "";
    condizione = false;
    cancella_list();
    cancella_dict();
}
// dealloca la lista e setta a nullptr i puntatori
void json::impl::cancella_list(){
    while(list_head){
        List aux = list_head;
        list_head = aux->next;
        delete aux;
    }
    list_head = list_tail = nullptr;
}
// dealloca la lista e setta a nullptr i puntatori
void json::impl::cancella_dict(){
    while(dict_head){
        Dict aux = dict_head;
        dict_head = aux->next;
        delete aux;
    }
    dict_head = dict_tail = nullptr;
}
// distruttore
json::~json(){
    pimpl->delete_everything(); // svuoto il contenuto di pimpl
    delete pimpl;   // dealloco pimpl
}
// su un json di tipo null, copia (deep) lo stato di rhs su this
void json::impl::copy(json const & rhs){
    //il json non ha liste, è già vuoto
    if (Tipo != null)
        throw json_exception {"la funzione copy va chiamata su oggetto di tipo null."};
    Tipo = rhs.pimpl->Tipo;
    stringa = rhs.pimpl->stringa;
    condizione = rhs.pimpl->condizione;
    numero = rhs.pimpl->numero;
    if (list_head != nullptr or list_tail != nullptr or dict_head != nullptr or dict_tail !=nullptr){
        throw json_exception {"la lista dovrebbe essere vuota, sto facendo una copia di oggetto json, ma *this non è null"};
    }
    copia_list(rhs.pimpl->list_head);
    copia_dict(rhs.pimpl->dict_head);
}
// copio una lista da un oggetto a this (vuoto)
void json::impl::copia_list(List const & rhs){
    if (rhs == nullptr){
        list_head = list_tail = nullptr;
        return;
    }
    //siccome la lista è già vuota, non devo eliminare nulla
    list_head = new Lista;
    list_head->info = rhs->info;
    list_head->next = nullptr;
    List aux = list_head;
    list_tail = list_head;
    List aux_rhs = rhs->next;
    while(aux_rhs){
        List nuova = new Lista;
        list_tail = aux = aux->next = nuova;
        aux->info = aux_rhs->info;
        aux->next = nullptr;
        aux_rhs = aux_rhs->next;
    }
}
// copio un dict da un oggetto a this (vuoto)
void json::impl::copia_dict(Dict const & rhs){
    if (rhs == nullptr){
        list_head = list_tail = nullptr;
        return;
    }
    dict_head = new Dizionario;
    dict_head->info = rhs->info;
    dict_head->next = nullptr;
    Dict aux = dict_head;
    dict_tail = dict_head;
    Dict aux_rhs = rhs->next;
    while(aux_rhs){
        Dict nuova = new Dizionario;
        dict_tail = aux = aux->next = nuova;
        aux->info = aux_rhs->info;
        aux->next = nullptr;
        aux_rhs = aux_rhs->next;
    }
}
// move semantic: move assignment
json& json::operator=(json&& rhs){
    if (this != &rhs) {
        pimpl->delete_everything();
        delete pimpl;
        this->pimpl = rhs.pimpl;
        rhs.pimpl = nullptr; // quando chiamo il distruttore non intacco il pimpl che ho spostato
    }
    return *this;
}
// move semantics: move constructor
json::json(json&& rhs){
    *this = std::move(rhs); 
}
// return true if this is json(list)
bool json::is_list() const {
    return pimpl->Tipo == impl::lis;
}
// return true if this is json(dict)
bool json::is_dictionary() const {
    return pimpl->Tipo == impl::diz;
}
// return true if this is json(string)
bool json::is_string() const {
    return pimpl->Tipo == impl::str;
}
// return true if this is json(number)
bool json::is_number() const {
    return pimpl->Tipo == impl::num;
}
// return true if this is json(bool)
bool json::is_bool() const {
    return pimpl->Tipo == impl::cond;
}
// return true if this is json(null)
bool json::is_null() const {
    return pimpl->Tipo == impl::null;
}
// se thsi è un json(number) ritorno il valore del double corrispondente
double& json::get_number(){
    if (not is_number())
        throw json_exception {"stai provando a prendere un numero da un json che non è di tipo number"};
    return pimpl->numero;
}
// se thsi è un json(number) ritorno il valore del double corrispondente (const)
double const& json::get_number() const {
    if (not is_number())
        throw json_exception {"stai provando a prendere un numero da un json che non è di tipo number"};
    return pimpl->numero;
}
