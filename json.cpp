#include "json.hpp"
#include <fstream>

using std::cout;
using std::cin;
using std::string;
using std::pair;
using std::endl;

char prova; // stores stream.peek(). I didn't want to redeclare it everytime
struct json::impl {
    enum tipologia {nullo, num, str, cond, lis, diz};
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
    void copy(json const & rhs);   // su un json di tipo null, copia (deep) lo stato di rhs su this
    void cancella_list();  // dealloca la lista e setta a nullptr i puntatori
    void cancella_dict();  // dealloca la lista e setta a nullptr i puntatori
    void copia_list(List const & rhs);// copio una lista da un oggetto a this (vuoto)
    void copia_dict(Dict const & rhs);

};
// default constructor = sets json obj to "null" type
json::json(){
    pimpl = new impl;
    pimpl->Tipo = pimpl->nullo;
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
    if (this){
        Tipo = nullo;
        numero = 0;
        stringa = "";
        condizione = false;
        cancella_list();
        cancella_dict();
    }
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
    if (Tipo != nullo)
        throw json_exception {"la funzione copy va chiamata su oggetto di tipo null."};
    Tipo = rhs.pimpl->Tipo;
    stringa = rhs.pimpl->stringa;
    condizione = rhs.pimpl->condizione;
    numero = rhs.pimpl->numero;
    if (list_head != nullptr || list_tail != nullptr || dict_head != nullptr || dict_tail !=nullptr){
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
    json::impl* aux = this->pimpl;
    if (this != &rhs) {
        this->pimpl = rhs.pimpl;
        rhs.pimpl = aux; // qu&&o chiamo il distruttore non intacco il pimpl che ho spostato
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
    return pimpl->Tipo == impl::nullo;
}
// se thsi è un json(number) ritorno il valore del double corrispondente
double& json::get_number(){
    if (not is_number())
        throw json_exception {"stai prov&&o a prendere un numero da un json che non è di tipo number"};
    return pimpl->numero;
}
// se thsi è un json(number) ritorno il valore del double corrispondente (const)
double const& json::get_number() const {
    if (not is_number())
        throw json_exception {"stai prov&&o a prendere un numero da un json che non è di tipo number"};
    return pimpl->numero;
}
// se this è un json(bool), ritorno il valore del bool corrispondente
bool& json::get_bool(){
    if (not is_bool())
        throw json_exception {"stai prov&&o a prendere un bool da un json che non è di tipo bool"};
    return pimpl->condizione;
}
// se this è un json(bool), ritorno il valore del bool corrispondente (const)
bool const& json::get_bool() const{
if (not is_bool())
        throw json_exception {"stai prov&&o a prendere un bool da un json che non è di tipo bool"};
    return pimpl->condizione;
}
// se this è un json(string), ritorno il valore del string corrispondente 
string& json::get_string() {
if (not is_string())
        throw json_exception {"stai prov&&o a prendere un string da un json che non è di tipo string"};
    return pimpl->stringa;
}
// se this è un json(string), ritorno il valore del string corrispondente (const)
string const& json::get_string() const{
if (not is_string())
        throw json_exception {"stai prov&&o a prendere un string da un json che non è di tipo string"};
    return pimpl->stringa;
}
// se this è un json(string), cambio il valore della string corrispondente. altrimenti, elimino tutto e lo faccio diventare stringa
void json::set_string(string const& x){
    pimpl->delete_everything();
    if (is_bool() || is_dictionary() || is_string() || is_number() || !is_null() || is_list()) 
        throw json_exception{"in set string, il delete everything non ritorna un null"};
    pimpl->Tipo = pimpl->str;
    pimpl->stringa = x;
    if (get_string() != x) 
        throw json_exception{"in set string, l'assegnamento ha qualcosa che non va"};
    if (is_bool() || is_dictionary() || !is_string() || is_number() || is_null() || is_list()) 
        throw json_exception{"in set string, il tipo non viene settato correttamente"};   
}
// se this è un json(bool), cambio il valore bool corrispondente. altrimenti, elimino tutto e lo faccio diventare bool
void json::set_bool(bool x){
    pimpl->delete_everything();
    if (is_bool() || is_dictionary() || is_string() || is_number() || !is_null() || is_list()) 
        throw json_exception{"in set bool, il delete everything non ritorna un null"};
    pimpl->Tipo = pimpl->cond;
    pimpl->condizione = x;
    if (get_bool() != x) 
        throw json_exception{"in set bool, l'assegnamento ha qualcosa che non va"};
    if (!is_bool() || is_dictionary() || is_string() || is_number() || is_null() || is_list()) 
        throw json_exception{"in set bool, il tipo non viene settato correttamente"};   
}
// se this è un json(num), cambio il valore num corrispondente. altrimenti, elimino tutto e lo faccio diventare num
void json::set_number(double x){
    pimpl->delete_everything();
    if (is_bool() || is_dictionary() || is_string() || is_number() || !is_null() || is_list()) 
        throw json_exception{"in set number, il delete everything non ritorna un null"};
    pimpl->Tipo = pimpl->num;
    pimpl->numero = x;
    if (get_number() != x) 
        throw json_exception{"in set num, l'assegnamento ha qualcosa che non va"};
    if (is_bool() || is_dictionary() || is_string() || !is_number() || is_null() || is_list()) 
        throw json_exception{"in set num, il tipo non viene settato correttamente"};   
}
// rendo this un json(null)
void json::set_null(){
    pimpl->delete_everything();
    if (is_bool() || is_dictionary() || is_string() || is_number() || !is_null() || is_list()) 
        throw json_exception{"in set null, il delete everything non ritorna un null"};
}
// rendo this un json(list) con lista vuota
void json::set_list(){
    pimpl->delete_everything();
    if (is_bool() || is_dictionary() || is_string() || is_number() || !is_null() || is_list()) 
        throw json_exception{"in set list, il delete everything non ritorna un null"};
    pimpl->Tipo = pimpl->lis;
    if (is_bool() || is_dictionary() || is_string() || is_number() || is_null() || !is_list()) 
        throw json_exception{"in set list, il tipo non viene settato correttamente"};   
}
// rendo this un json(list) con lista vuota
void json::set_dictionary(){
    pimpl->delete_everything();
    if (is_bool() || is_dictionary() || is_string() || is_number() || !is_null() || is_list()) 
        throw json_exception{"in set dict, il delete everything non ritorna un null"};
    pimpl->Tipo = pimpl->diz;
    if (is_bool() || !is_dictionary() || is_string() || is_number() || is_null() || is_list()) 
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
    if (this->is_list()){
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
        while(aux != nullptr && !found) {
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
        while(aux != nullptr && !found) {
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
// struttura iteratore lista
struct json::list_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;

        list_iterator(impl::List);
        json& operator*() const;
        json* operator->() const;
        list_iterator& operator++();    // ++it
        list_iterator operator++(int);  // it++

        bool operator==(list_iterator const&) const; //it1 == it2
        bool operator!=(list_iterator const&) const; //it1 != it2
        operator bool() const; //casting comparativo (bool)
    private:
        impl::List it_ptr; 
};
// struttura iteratore dizionario
struct json::dictionary_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;

        dictionary_iterator(impl::Dict);
        pair<string, json>& operator*() const;
        pair<string, json>* operator->() const;
        dictionary_iterator& operator++();    // ++it
        dictionary_iterator operator++(int);  // it++

        bool operator==(dictionary_iterator const&) const; //it1 == it2
        bool operator!=(dictionary_iterator const&) const; //it1 != it2
        operator bool() const; //casting comparativo (bool)
    private:
        impl::Dict it_ptr; 
};
// struttura iteratore lista const
struct json::const_list_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;

        const_list_iterator(impl::List);
        json& operator*() const;
        json* operator->() const;
        const_list_iterator& operator++();    // ++it
        const_list_iterator operator++(int);  // it++

        bool operator==(const_list_iterator const&) const; //it1 == it2
        bool operator!=(const_list_iterator const&) const; //it1 != it2
        operator bool() const; //casting comparativo (bool)
    private:
        impl::List it_ptr; 
};
// struttura iteratore dizionario const
struct json::const_dictionary_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;

        const_dictionary_iterator(impl::Dict);
        pair<string, json>& operator*() const;
        pair<string, json>* operator->() const;
        const_dictionary_iterator& operator++();    // ++it
        const_dictionary_iterator operator++(int);  // it++

        bool operator==(const_dictionary_iterator const&) const; //it1 == it2
        bool operator!=(const_dictionary_iterator const&) const; //it1 != it2
        operator bool() const; //casting comparativo (bool)
    private:
        impl::Dict it_ptr; 
};
//costruttore default list_iterator
json::list_iterator::list_iterator(impl::List p) : it_ptr(p) {}
//costruttore default const_list_iterator
json::const_list_iterator::const_list_iterator(impl::List p) : it_ptr(p) {}
//costruttore default dict_iterator
json::dictionary_iterator::dictionary_iterator(impl::Dict p) : it_ptr(p) {}
//costruttore default const_dict_iterator
json::const_dictionary_iterator::const_dictionary_iterator(impl::Dict p) : it_ptr(p) {}
//operatore di dereferenziazione list_iterator
json& json::list_iterator::operator*() const{
    return it_ptr->info;
}
//operatore di dereferenziazione const_list_iterator
json& json::const_list_iterator::operator*() const{
    return it_ptr->info;    
}
//operatore di dereferenziazione dictionary_iterator
pair<string, json>& json::dictionary_iterator::operator*() const{
    return it_ptr->info;    
}
//operatore di dereferenziazione const_dictionary_iterator 
pair<string, json>& json::const_dictionary_iterator::operator*() const{
    return it_ptr->info;    
}
//operatore-> const_list_iterator
json* json::list_iterator::operator->() const{
    return &(it_ptr->info);
}
//operatore-> const_list_iterator
json* json::const_list_iterator::operator->() const{
    return &(it_ptr->info);    
}
//operatore-> dictionary_iterator
pair<string, json>* json::dictionary_iterator::operator->() const{
    return &(it_ptr->info);    
}
//operatore-> const_dictionary_iterator 
pair<string, json>* json::const_dictionary_iterator::operator->() const{
    return &(it_ptr->info);    
}
// prefix increment per list_iterator
json::list_iterator& json::list_iterator::operator++(){
    it_ptr = it_ptr->next;
	return *this;
}
// prefix increment per const_list_iterator
json::const_list_iterator& json::const_list_iterator::operator++(){
    it_ptr = it_ptr->next;
	return *this;
}
// prefix increment per dicttionary_iterator
json::dictionary_iterator& json::dictionary_iterator::operator++(){
    it_ptr = it_ptr->next;
	return *this;
}
// prefix increment per const_dictionary_iterator
json::const_dictionary_iterator& json::const_dictionary_iterator::operator++(){
    it_ptr = it_ptr->next;
	return *this;
}
// postfix increment per list_iterator
json::list_iterator json::list_iterator::operator++(int){
	list_iterator it = {it_ptr};
	++(*this);
	return it;
}
// postfix increment per const_list_iterator
json::const_list_iterator json::const_list_iterator::operator++(int){
	const_list_iterator it = {it_ptr};
	++(*this);
	return it;
}
// postfix increment per dicttionary_iterator
json::dictionary_iterator json::dictionary_iterator::operator++(int){
	dictionary_iterator it = {it_ptr};
	++(*this);
	return it;
}
// postfix increment per const_dictionary_iterator
json::const_dictionary_iterator json::const_dictionary_iterator::operator++(int){
	const_dictionary_iterator it = {it_ptr};
	++(*this);
	return it;
}
// operator== per list_iterator: ritorna true iff it1 e it2 puntano alla stessa area di memoria 
bool json::list_iterator::operator==(list_iterator const& rhs) const{
	return it_ptr == rhs.it_ptr;
}
// operator== per dict_iterator: ritorna true iff it1 e it2 puntano alla stessa area di memoria 
bool json::dictionary_iterator::operator==(dictionary_iterator const& rhs) const{
	return it_ptr == rhs.it_ptr;
}
// operator== per const_list_iterator: ritorna true iff it1 e it2 puntano alla stessa area di memoria 
bool json::const_list_iterator::operator==(const_list_iterator const& rhs) const{
	return it_ptr == rhs.it_ptr;
}
// operator== per const_dict_iterator: ritorna true iff it1 e it2 puntano alla stessa area di memoria 
bool json::const_dictionary_iterator::operator==(const_dictionary_iterator const& rhs) const{
	return it_ptr == rhs.it_ptr;
}
// operator!= per list_iterator: ritorna true iff it1 e it2 non puntano alla stessa area di memoria 
bool json::list_iterator::operator!=(list_iterator const& rhs) const{
	return it_ptr != rhs.it_ptr;
}
// operator!= per dict_iterator: ritorna true iff it1 e it2 non puntano alla stessa area di memoria 
bool json::dictionary_iterator::operator!=(dictionary_iterator const& rhs) const{
	return it_ptr != rhs.it_ptr;
}
// operator!= per const_list_iterator: ritorna true iff it1 e it2 non  puntano alla stessa area di memoria 
bool json::const_list_iterator::operator!=(const_list_iterator const& rhs) const{
	return it_ptr != rhs.it_ptr;
}
// operator!= per const_dict_iterator: ritorna true iff it1 e it2 non puntano alla stessa area di memoria 
bool json::const_dictionary_iterator::operator!=(const_dictionary_iterator const& rhs) const{
	return it_ptr != rhs.it_ptr;
}
//eseguo casting a bool su list_iterator
json::list_iterator::operator bool() const{
    return it_ptr != nullptr;
}
//eseguo casting a bool su const_list_iterator
json::const_list_iterator::operator bool() const{
    return it_ptr != nullptr;
}
//eseguo casting a bool su dict_iterator
json::dictionary_iterator::operator bool() const{
    return it_ptr != nullptr;
}
//eseguo casting a bool su const_dictionary_iterator
json::const_dictionary_iterator::operator bool() const{
    return it_ptr != nullptr;
}
// iteratore begin() su list_iterator
json::list_iterator json::begin_list(){
    return list_iterator {json::pimpl->list_head};
}
// iteratore begin() su const_list_iterator
json::const_list_iterator json::begin_list() const{
    return const_list_iterator {json::pimpl->list_head};
}
// iteratore end() su list_iterator
json::list_iterator json::end_list(){
    return list_iterator {json::pimpl->list_tail};

}
// iteratore end() su constlist_iterator
json::const_list_iterator json::end_list() const{
    return const_list_iterator {json::pimpl->list_tail->next};

}
// iteratore begin() su dict_iterator
json::dictionary_iterator json::begin_dictionary(){
    return dictionary_iterator{json::pimpl->dict_head};

}
// iteratore begin() su onst_dict_iterator
json::const_dictionary_iterator json::begin_dictionary() const{
    return const_dictionary_iterator{json::pimpl->dict_head};
}
// iteratore end() su dict_iterator
json::dictionary_iterator json::end_dictionary(){
    return dictionary_iterator{json::pimpl->dict_tail->next};
}
// iteratore end() su const_dict_iterator
json::const_dictionary_iterator json::end_dictionary() const{
    return const_dictionary_iterator{json::pimpl->dict_tail};
}
/**
 * GRAMMATICA:
 * pongo µ = vuoto/null
 * T ->     number  |   true    |   false
 * J ->     [L] |   {D}     |   "S"     |   T   | SOLO IN CASO ESTERNO : µ
 * S ->     µ   |   stringa |    S\"S
 * L ->     µ   |   J       |   J,L     |   T   |   T,L
 * D ->     "S":J   |   "S":J,D  |   µ
 * terminali: null, number, true, false, stringa.
 * non terminali: S (stringa), L (lista), D (dizionario), J (json)
*/

//Lancia una eccezione di parsing in formato st&&ard
void error_handler(string exp /*carattere/i che mi aspettavo di leggere*/, char obt /*cosa ho ottenuto*/, uint64_t pos){
    string errore{"Sono in posizione "};
    errore.append(std::to_string(pos));
    errore.append(". Mi aspettavo qualcosa come: ");
    errore+=exp;
    errore.append(", ma ho ottenuto: ");
    errore+=obt;
    throw json_exception {errore};
}
//forward declarations. pos indica il numero di caratteri estratti per ogni chiamata.
//mi è particolarmente utile per vedere errori e il caso di inizio file

//parsa i tipi terminali
json JTERM (std::istream& rhs, uint64_t& pos);
//parsa i tipi stringa
string JSTRING (std::istream& rhs, uint64_t& pos);
//parsa i tipi lista
json JLIST (std::istream& rhs, uint64_t& pos);
//parsa i tipi dizionario
json JDICT (std::istream& rhs, uint64_t& pos);
//parsa i tipi json
json J (std::istream& rhs, uint64_t& pos);
//parsa i pair del dizionario
pair<string, json> DICT (std::istream& rhs, uint64_t&);

/*returns true if char c is NOT a valid initial character of Json type*/
bool controllo_carattere_errore_j(char c){
    if ((c != '-') &&  (c != '.') && not (c >= '0' && c <= '9') &&
    (c != '+') &&  (c != '[') && (c != 't') && (c != 'f') && (c != '{') && (c != 'n'))
        return true;
    else return false;
}
// Jason generico: J ->  µ | number | [L] | true | false | {D} | "S"
json J (std::istream& rhs, uint64_t& pos){
    /**
     * per prima cosa gestisco il caso più esterno.
     * 
     * 
     * In generale, tutti i casi "lista vuota" "dizionario vuoto" e "stringa vuota" sono
     * controllati da D, S, L rispettivamente, quindi escluso il caso iniziale di json null, 
     * non ho da controllare se ho un valore vuoto. 
     * Viene fatto dalle funzioni controll&&o se il carattere successivo è ] o } o ", 
     * e in quel caso manco viene chiamato J 
    */
    json j;
    char c;
    
    if (pos == 0 /*non ho ancora estratto nulla*/) {
        prova = rhs.peek();
        if (prova == -1) {
            return j;
        }
        rhs>>c;
        if (c != '[' && c!='{' && c != '"') {
            rhs.putback(c);
            j = JTERM(rhs, pos);    //già qui dentro controllo la correttezza dei caratteri iniziali
            cout<<"ultimo carattere: "<<rhs.peek()<<endl;
        }
        else if (c == '"') {
            j.set_string(JSTRING(rhs, ++pos));
            prova = rhs.peek();
            if (prova == -1) error_handler("\"", EOF, pos);
            rhs>>c;
            pos++;
            if (c != '"') error_handler("\"", c, pos);
        }
        else {
            j = c=='[' ? JLIST(rhs, ++pos) : JDICT(rhs, ++pos); //mi rimane controllare i casi lista e dizionario
            prova = rhs.peek();
            if (prova == -1) error_handler("] o }", EOF, pos);
            char c2;
            rhs>>c2;
            pos++;
            if (c == '[' && c2 != ']') error_handler("]", c, pos);
            if (c == '{' && c2 != '}') error_handler("}", c, pos);
        }
        //cout<<"carattere attuale: "<<rhs.peek()<<" ";
        prova = rhs.peek();
        if (prova != -1) {
            error_handler("il file deve finire qui", c, pos);
        }
    }
    else {
        prova = rhs.peek();
        if (prova == -1) error_handler("un carattere qualsiasi", EOF, pos);

        rhs>>c;
        
        if (controllo_carattere_errore_j(c)){
            error_handler("un carattere di inizio di json, vedi commento", c, pos);
        }
        if (c == '[') {
            ++pos;  //consumo [
            j = JLIST(rhs, pos); //input di lista
            rhs>>c; pos++;  //consumo ]
            if (c != ']') error_handler("carattere di fine lista ] ", c, pos);
        }
        else if (c == '{') {
            ++pos;
            j = JDICT(rhs,pos);
            rhs>>c; pos++;
            if (c != '}') error_handler("carattere di fine dizionario } ", c, pos);
        }
        else if (c == '"'){
            ++pos;
            j.set_string(JSTRING(rhs, pos));
            rhs>>c; pos++;
            if (c != '"') error_handler("carattere di fine stringa \"", c, pos);
        }
        else {
            // caso Terminale
            rhs.putback(c);
            j = JTERM(rhs, pos);
        }
    }
    cout<<"posizione "<<pos;
    return j;
}
// Json List: L ->    lista di json
json JLIST (std::istream& rhs, uint64_t& pos){
    
    json j; // mi preparo il mio json
    j.set_list(); // lo rendo una lista
    
    prova = rhs.peek();
    if (prova == -1) error_handler("sono in una lista, non posso trovarmi eof!", EOF, pos);
    char c;
    rhs>>c;
    rhs.putback(c);
    // arrivo a questo punto che ho lo stream sul primo carattere dopo [ ([ è l'ultimo carattere consumato)
    while (c != ']'){
        cout<<endl<<"sono nel ciclo. iterazione"<<endl;
                
        if (controllo_carattere_errore_j(c)) error_handler("un carattere di inizio json", c, pos); // non può esserci una virgola xk non esistono elementi vuoti
        
        j.push_back(J(rhs, pos)); // appendo alla lista il prossimo elemento json.
        // faccio i controlli come sopra.
        prova = rhs.peek();
        if (prova == -1) error_handler("sono in una lista, non posso trovarmi eof!", EOF, pos);
        cout<<" sono in entro?. cosa vedo nello stream? "<<prova<<" ."<<" cosa contiene invece c? "<<c<<endl;

        //mi serve prendere il carattere successivo perché devo controllarlo nel while.
        rhs>>c;
        cout<<" butto dentro anche c. cosa vedo nello stream? "<<(char)rhs.peek()<<" ."<<" cosa contiene invece c? "<<c<<endl;

        if (c != ']' && c != ',') error_handler("] o , ", c, pos);
        else if (c == ',') {pos++; c = rhs.peek();} // devo consumare la , e preparare c per l'iterazione successva
        else if (c == ']') {
            //l'ho preso, ma non devo consumarlo, perché se ne occupa il ciclo.
            rhs.putback(c);
        }
    }
    //Mi trovo ora che ho appeso tutti gli elementi necessari, 
    prova = rhs.peek();
    if (prova == -1) error_handler("sono in una lista, non posso trovarmi eof!", EOF, pos);
    //non devo consumare ] perché se ne occupa il chiamante.
    return j;
}
// Json DICT: D ->    lista di DICT
json JDICT (std::istream& rhs, uint64_t& pos){
    // è praticamente un copincolla della lista
    json j;
    j.set_dictionary(); // lo rendo una lista
    prova = rhs.peek();
    if (prova == -1) error_handler("sono in un dizionario, non posso trovarmi eof!", EOF, pos);
    char c;
    rhs>>c;
    rhs.putback(c);
    // arrivo a questo punto che ho lo stream sul primo carattere dopo { ({ è l'ultimo carattere consumato)
    while (c != '}'){
        if (controllo_carattere_errore_j(c) && c!=',' && c != ':') error_handler("un carattere di inizio json o una , o i :", c, pos);
        else if (c == ',') {
            pos++; rhs>>c; // consumo la virgola
            j.insert(pair<string, json>{string{}, json{}}); // appendo un valore pair vuoto
            prova = rhs.peek();
            if (prova == -1) error_handler("sono in un dizionario, non posso trovarmi eof!", EOF, pos);
            //mi serve prendere il carattere successivo perché devo controllarlo nel while.
            rhs>>c;
            if (c != ']' && c != ',' && c != ':') error_handler("] o , : ", c, pos);
            if (c == ']') {
                //l'ho preso, ma non devo consumarlo, perché se ne occupa il ciclo.
                rhs.putback(c);
            }
            // altrimenti, è giusto che venga consumato, così allanuova iterazione rhs punta all'elemento json successivo.
        }
        else {
            j.insert(DICT(rhs, pos)); // appendo alla lista il prossimo elemento json.
            // faccio i controlli come sopra.
            prova = rhs.peek();
            if (prova == -1) error_handler("sono in un dizionario, non posso trovarmi eof!", EOF, pos);
            //mi serve prendere il carattere successivo perché devo controllarlo nel while.
            rhs>>c;
            if (c != ']' && c != ',') error_handler("] o , ", c, pos);
            if (c == ']') {
                //l'ho preso, ma non devo consumarlo, perché se ne occupa il ciclo.
                rhs.putback(c);
            }
            // altrimenti, è giusto che venga consumato, così allanuova iterazione rhs punta all'elemento json successivo.
        }
    }
    //Mi trovo ora che ho appeso tutti gli elementi necessari, 
    prova = rhs.peek();
    if (prova == -1) error_handler("sono in una lista, non posso trovarmi eof!", EOF, pos);
    //non devo consumare } perché se ne occupa il chiamante.
    return j;
}
// pair DICT: DICT -> µ:µ o "S":J
pair<string, json> DICT (std::istream& rhs, uint64_t& pos){
    pair<string, json> p;
    char c;
    rhs>>c; rhs.putback(c);
    if (c != ':' && c != '"') error_handler(" : o \" ", c, pos);
    if (c == ':' ) {
        p.first = "";  // stringa vuota
        pos++; rhs>>c; // consumo i :
    }
    else {
        rhs>>c; pos++; // consumo la " di apertura
        prova = rhs.peek();
        if (prova == -1) error_handler("sto pars&&o DICT, non posso trovarmi eof!", EOF, pos);
        p.first = JSTRING(rhs, pos);
        prova = rhs.peek();
        if (prova == -1) error_handler("sto pars&&o DICT, non posso trovarmi eof!", EOF, pos);
        rhs>>c; pos++; // consumo la " di chiusura
        prova = rhs.peek();
        if (prova == -1) error_handler("sto pars&&o DICT, non posso trovarmi eof!", EOF, pos);
        rhs>>c; pos++; // consumo i :
    }
    // arrivo qui coi : già consumati (devo ancora controllare però che siano effettivamente :)
    if (c != ':') error_handler(": ", c, pos);
    prova = rhs.peek();
    if (prova == -1) error_handler("sto pars&&o DICT, non posso trovarmi eof!", EOF, pos);
    rhs>>c; rhs.putback(c); //solo per controllo
    if (controllo_carattere_errore_j(c) && c != '}' && c != ',') error_handler("carattere di inizio J o } o ,", c, pos);
    if (c == '}' || c == ',') {
        p.second = json{};
    }
    else {
        p.second = J(rhs, pos);
    }
    return p;
}
// JTERM: number  |   true    |   false     |   null
json JTERM(std::istream& rhs, uint64_t& pos){
    // arrivo in JTERM con rhs che punta al primo carattere in modo corretto.
    prova = rhs.peek();
    if (prova == -1) error_handler("sto pars&&o un terminale, non posso trovarmi eof!", EOF, pos);
    char c;
    rhs>>c;
    rhs.putback(c);
    json j;
    if (c=='}' || c==']' || c==',') return j;
    if (c=='n') {
        char a[] = "null";
        for(short i = 0; i<4; i++){
            rhs>>c; pos++;
            if (c != a[i]) error_handler(" la scritta null. in una posizione ", c, pos);
        }
    }
    else if ((c >= '0' && c <= '9') || c == '.' || c=='-' || c=='+') {
        double a;
        rhs>>a;
        string s = std::to_string(a);
        //string s {"12"}; 
        pos += s.length()-1;
        j.set_number(a);
    }
    else if (c == 'f'){
        char a[] = "false";
        for(short i = 0; i<5; i++){
            rhs>>c; pos++;
            if (c != a[i]) error_handler(" la scritta false. in una posizione ", c, pos);
        }
        j.set_bool(false);
    }
    else if (c == 't'){
        char a[] = "true";
        for(short i = 0; i<4; i++){
            rhs>>c; pos++;
            if (c != a[i]) error_handler(" la scritta true. in una posizione ", c, pos);
        }
        j.set_bool(false);
    }
    return j;
}
// JSTRING: µ   |   stringa |    S\"S
string JSTRING (std::istream& rhs, uint64_t& pos){
    //arrivo qui con la " già consumata
    prova = rhs.peek();
    if (prova == -1) error_handler("stringa, sono in EOF", EOF, pos);
    string s;
    char c = 0;
    while (c != '"'){
        prova = rhs.peek();
        if (prova == -1) error_handler("stringa, sono in EOF", EOF, pos);
        rhs>>c;
        if (c == '"') rhs.putback(c);
        else {
            prova = rhs.peek();
            if (prova == -1) error_handler("stringa, sono in EOF", EOF, pos);
            pos++;
            s += c;
            if (c == '\\'){
                rhs>>c; pos++; //consumo anche il carattere " dopo
                s += c;
                rhs>>c; // per evitare problemi in guardia, guardo già all'iterazione successiva
                if (c == '"') rhs.putback(c);
                else {
                    pos++;
                    s += c;
                }
            }
        }

    }
    return s; //ho già rimesso nello stream la ".
}

std::ostream& operator<<(std::ostream& lhs, json const& rhs){
    if (rhs.is_bool()) lhs<<rhs.get_bool();
    else if (rhs.is_string()) lhs<<"\""<<rhs.get_string()<<"\"";
    else if (rhs.is_null()) lhs<<"null";
    else if (rhs.is_number()) lhs<<rhs.get_number();
    else if (rhs.is_dictionary()) {
        lhs<<"{";
        for(auto it = rhs.begin_dictionary(); it != rhs.end_dictionary(); it++){
            lhs<<"\""<<it->first<<"\":"<<it->second;
        }
        lhs<<"}";
    }
    else if (rhs.is_list()){
        lhs<<"[";
        for(auto it = rhs.begin_list(); it != rhs.end_list(); it++){
            lhs<<*it;
        }
        lhs<<"]";
    }
    else throw json_exception {"qualcosa non va nell'ostream"};
    return lhs;
}

// operatore di input, da dove parte il tutto
std::istream& operator>>(std::istream& lhs, json& rhs){
    try {
        uint64_t pos = 0;
        rhs = J(lhs, pos);
    } catch (const json_exception& e) {
        throw e;
    }
    return lhs;
}

int main() {
    std::ifstream file("myfile.json");
    if (!file) {
        std::cerr << "Errore nell'apertura del file." << std::endl;
        return 1;
    }

    try {
        json j;
        file >> j;

        std::cout <<endl<< "JSON letto:\n" << j << std::endl;
    } catch (const json_exception& e) {
        std::cerr << "Errore nel parsing del JSON: " << e.msg << std::endl;
        return 1;
    }

    return 0;
}
