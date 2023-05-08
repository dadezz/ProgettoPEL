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
// se this è un json(bool), ritorno il valore del bool corrispondente
bool& json::get_bool(){
    if (not is_bool())
        throw json_exception {"stai provando a prendere un bool da un json che non è di tipo bool"};
    return pimpl->condizione;
}
// se this è un json(bool), ritorno il valore del bool corrispondente (const)
bool const& json::get_bool() const{
if (not is_bool())
        throw json_exception {"stai provando a prendere un bool da un json che non è di tipo bool"};
    return pimpl->condizione;
}
// se this è un json(string), ritorno il valore del string corrispondente 
string& json::get_string() {
if (not is_string())
        throw json_exception {"stai provando a prendere un string da un json che non è di tipo string"};
    return pimpl->stringa;
}
// se this è un json(string), ritorno il valore del string corrispondente (const)
string const& json::get_string() const{
if (not is_string())
        throw json_exception {"stai provando a prendere un string da un json che non è di tipo string"};
    return pimpl->stringa;
}
// se this è un json(string), cambio il valore della string corrispondente. altrimenti, elimino tutto e lo faccio diventare stringa
void json::set_string(string const& x){
    pimpl->delete_everything();
    if (is_bool() or is_dictionary() or is_string() or is_number() or !is_null() or is_list()) 
        throw json_exception{"in set string, il delete everything non ritorna un null"};
    pimpl->Tipo = pimpl->str;
    pimpl->stringa = x;
    if (get_string() != x) 
        throw json_exception{"in set string, l'assegnamento ha qualcosa che non va"};
    if (is_bool() or is_dictionary() or !is_string() or is_number() or is_null() or is_list()) 
        throw json_exception{"in set string, il tipo non viene settato correttamente"};   
}
// se this è un json(bool), cambio il valore bool corrispondente. altrimenti, elimino tutto e lo faccio diventare bool
void json::set_bool(bool x){
    pimpl->delete_everything();
    if (is_bool() or is_dictionary() or is_string() or is_number() or !is_null() or is_list()) 
        throw json_exception{"in set bool, il delete everything non ritorna un null"};
    pimpl->Tipo = pimpl->cond;
    pimpl->condizione = x;
    if (get_bool() != x) 
        throw json_exception{"in set bool, l'assegnamento ha qualcosa che non va"};
    if (!is_bool() or is_dictionary() or is_string() or is_number() or is_null() or is_list()) 
        throw json_exception{"in set bool, il tipo non viene settato correttamente"};   
}
// se this è un json(num), cambio il valore num corrispondente. altrimenti, elimino tutto e lo faccio diventare num
void json::set_number(double x){
    pimpl->delete_everything();
    if (is_bool() or is_dictionary() or is_string() or is_number() or !is_null() or is_list()) 
        throw json_exception{"in set number, il delete everything non ritorna un null"};
    pimpl->Tipo = pimpl->num;
    pimpl->numero = x;
    if (get_number() != x) 
        throw json_exception{"in set num, l'assegnamento ha qualcosa che non va"};
    if (is_bool() or is_dictionary() or is_string() or !is_number() or is_null() or is_list()) 
        throw json_exception{"in set num, il tipo non viene settato correttamente"};   
}
// rendo this un json(null)
void json::set_null(){
    pimpl->delete_everything();
    if (is_bool() or is_dictionary() or is_string() or is_number() or !is_null() or is_list()) 
        throw json_exception{"in set null, il delete everything non ritorna un null"};
}
// rendo this un json(list) con lista vuota
void json::set_list(){
    pimpl->delete_everything();
    if (is_bool() or is_dictionary() or is_string() or is_number() or !is_null() or is_list()) 
        throw json_exception{"in set list, il delete everything non ritorna un null"};
    pimpl->Tipo = pimpl->lis;
    if (is_bool() or is_dictionary() or is_string() or is_number() or is_null() or !is_list()) 
        throw json_exception{"in set list, il tipo non viene settato correttamente"};   
}
// rendo this un json(list) con lista vuota
void json::set_dictionary(){
    pimpl->delete_everything();
    if (is_bool() or is_dictionary() or is_string() or is_number() or !is_null() or is_list()) 
        throw json_exception{"in set dict, il delete everything non ritorna un null"};
    pimpl->Tipo = pimpl->diz;
    if (is_bool() or !is_dictionary() or is_string() or is_number() or is_null() or is_list()) 
        throw json_exception{"in set dict, il tipo non viene settato correttamente"};   
}
// se this è json(list), aggiungo x in testa, altrimenti throw exception
void json::push_front(json const& x){
    if (not is_list()){
        if (pimpl->list_head == nullptr) {
            pimpl->list_head = pimpl->list_tail = new impl::Lista;
            pimpl->list_head->next = nullptr;
            pimpl->list_head->info = x;
        }
        else {
            impl::List nuova = new impl::Lista;
            nuova->info = x;
            nuova->next = pimpl->list_head;
            pimpl->list_head = nuova;
        }
    }
    else throw json_exception {"stai facendo una  push front in un json di tipo non list"};
}
// se this è json(list), aggiungo x in coda, altrimenti throw exception
void json::push_back(json const& x){
    if (not is_list()){
        if (pimpl->list_head == nullptr) {
            pimpl->list_head = pimpl->list_tail = new impl::Lista;
            pimpl->list_head->next = nullptr;
            pimpl->list_head->info = x;
        }
        else {
            impl::List nuova = new impl::Lista;
            nuova->info = x;
            nuova->next = nullptr;
            pimpl->list_tail = pimpl->list_tail->next = nuova;
        }
    }
    else throw json_exception {"stai facendo una  push back in un json di tipo non list"};
}
// se this è json(dict), aggiungo x. altrimenti throw exception
// non c'è bisogno di controllare se esiste già una coppia nel dizionario la cui chiave è x.first
void json::insert(pair<string,json> const& x){
    // lo inserisco in testa
    if (not is_dictionary()){
        if (pimpl->dict_head == nullptr) {
            pimpl->dict_head = pimpl->dict_tail = new impl::Dizionario;
            pimpl->dict_head->next = nullptr;
            pimpl->dict_head->info = x;
        }
        else {
            impl::Dict nuova = new impl::Dizionario;
            nuova->info = x;
            nuova->next = pimpl->dict_head;
            pimpl->dict_head = nuova;
        }
    }
    else throw json_exception {"stai facendo una insert in un json di tipo non dict"};
}
// metodi per accedere dall'esterno al contenuto del container (const)
json const& json::operator[](std::string const& key) const{
    if (is_dictionary()){
        impl::Dict aux = pimpl->dict_head;
        bool found = false;
        while(aux != nullptr and !found) {
            if (aux->info.first == key) {
                found = true;
            }
            else aux = aux->next;
        }
        if (found) return aux->info.second;
        else throw json_exception {"non puoi aggiungere una copia chiave valore nel dizionario in un contesto const"};
    }
    else throw json_exception {"l'operator [] funziona solo su json di tipo dict"};

}
// metodo per accedere dall'esterno al contenuto del container
json& json::operator[](std::string const& key){
    if (is_dictionary()){
        impl::Dict aux = pimpl->dict_head;
        bool found = false;
        while(aux != nullptr and !found) {
            if (aux->info.first == key) {
                found = true;
            }
            else aux = aux->next;
        }
        if (found) return aux->info.second;
        else {
            insert(pair<string, json> {key, json{}}); // lo inserisce in testa
            return pimpl->dict_head->info.second;
        }
    }
    else throw json_exception {"l'operator [] funziona solo su json di tipo dict"};
}


