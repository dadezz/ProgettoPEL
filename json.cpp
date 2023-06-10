#include "json.hpp"
#include <fstream>

using std::cout;
using std::cin;
using std::string;
using std::pair;
using std::endl;

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
    Tipo = nullo;
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
    if (this->Tipo != nullo)
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
        aux->info = aux_rhs->info;   // [1 | ->2]       [2 | ->[]]      [ [3 | ->4] [4 | ->nullptr]    |    ->nullptr  ]
        aux->next = nullptr;
        aux_rhs = aux_rhs->next;
    }
}
// copio un dict da un oggetto a this (vuoto)
void json::impl::copia_dict(Dict const & rhs){
    if (rhs == nullptr){
        dict_head = dict_tail = nullptr;
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
    if (is_dictionary()){
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
    return nullptr;

}
// iteratore end() su constlist_iterator
json::const_list_iterator json::end_list() const{
    return nullptr;
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
    return nullptr;
}
// iteratore end() su const_dict_iterator
json::const_dictionary_iterator json::end_dictionary() const{
    return nullptr;
}

//Lancia una eccezione di parsing in formato st&&ard
void error_handler(string exp /*carattere/i che mi aspettavo di leggere*/, char obt /*cosa ho ottenuto*/){
    string errore{". Mi aspettavo qualcosa come: "};
    errore+=exp;
    errore.append(", ma ho ottenuto: ");
    errore+=obt;
    throw json_exception {errore};
}
//forward declarations. pos indica il numero di caratteri estratti per ogni chiamata.
//mi è particolarmente utile per vedere errori e il caso di inizio file

//parsa i tipi terminali
json JTERM (std::istream& rhs);
//parsa i tipi stringa
string JSTRING (std::istream& rhs);
//parsa i tipi lista
json JLIST (std::istream& rhs);
//parsa i tipi dizionario
json JDICT (std::istream& rhs);
//parsa i tipi json
json J (std::istream& rhs);
//parsa i pair del dizionario
pair<string, json> DICT (std::istream& rhs);

/*returns true if char c is NOT a valid initial character of Json type*/
bool controllo_carattere_errore_j(char c){
    if ((c != '-') &&  (c != '.') && not (c >= '0' && c <= '9') && (c != '+') &&  (c != '[') && (c != 't') && (c != 'f') && (c != '{') && (c != 'n') && (c != '"'))
        return true;
    else return false;
}
// Jason generico: J ->  µ | number | [L] | true | false | {D} | "S"
json J (std::istream& rhs){

    json j;
    char c;

    c = rhs.peek();
    if (c == -1) error_handler("un carattere qualsiasi", EOF);

    rhs>>c;
    
    if (controllo_carattere_errore_j(c)){
        error_handler("un carattere di inizio di json, vedi commento", c);
    }
    if (c == '[') {
        //già consumato [
        j = JLIST(rhs); //input di lista
        rhs>>c; //consumo ]
        if (c != ']') error_handler("carattere di fine lista ] ", c);
    }
    else if (c == '{') {
        //già consumato {
        j = JDICT(rhs);
        rhs>>c; //consumo }
        if (c != '}') error_handler("carattere di fine dizionario } ", c);
    }
    else if (c == '"'){
        //già consumato "
        j.set_string(JSTRING(rhs));
        rhs>>c; //consumo "
        if (c != '"') error_handler("carattere di fine stringa \"", c);
    }
    else {
        // caso Terminale: non devo consumare caratteri
        rhs.putback(c);
        j = JTERM(rhs);
    }
    return j;
}


// Json List: L ->    lista di json
json JLIST (std::istream& rhs){
    
    json j; // mi preparo il mio json
    j.set_list(); // lo rendo una lista
    
    char c = rhs.peek();
    if (c == -1) error_handler("sono in una lista, non posso trovarmi eof!", EOF);

    // arrivo a questo punto che ho lo stream sul primo carattere dopo [ ([ è l'ultimo carattere consumato)
    while (c != ']'){       
        if (controllo_carattere_errore_j(c)) error_handler("un carattere di inizio json", c); // non può esserci una virgola xk non esistono elementi vuoti
        json aux = J(rhs);
        j.push_back(aux); // appendo alla lista il prossimo elemento json.

        // faccio i controlli come sopra.
        // mi aspetto di trovare o una , o una ]. solo nel caso della virgola devo consumarla per poi tornare a inizio ciclo
        c = rhs.peek();
        if (c == -1) error_handler("sono in una lista, non posso trovarmi eof!", EOF);
        if (c != ']' && c != ',') error_handler("] o , ", c);


        else if (c == ',') {
            rhs>>c; //la consumo effettivamente. notre che il valore contenuto in c non cambia
            c = rhs.peek(); // preparo il carattere per l'iterazione successiva. é necessario controllare ora
                        // che il carattere successivo non sia una ].
            if (c == ']') error_handler ("carattere di inizio json in una lista, ", c);
        }
        else if (c == ']') ; //non faccio nulla perché il carattere è ] e non è stato consumato
    }
    //Mi trovo ora che ho appeso tutti gli elementi necessari
    //non devo consumare ] perché se ne occupa il chiamante.
    return j;
}

// Json DICT: D ->    lista di DICT
json JDICT (std::istream& rhs){
    // è praticamente un copincolla della lista
    json j;
    j.set_dictionary(); // lo rendo un dizionario

    char c = rhs.peek();
    if (c == -1) error_handler("sono in un dizionario, non posso trovarmi eof!", EOF);

    // arrivo a questo punto che ho lo stream sul primo carattere dopo { ({ è l'ultimo carattere consumato)
    while (c != '}'){

        //mi aspetto quindi obbligatoriamente un inizio stringa.
        if (c != '"') error_handler("in dizionario, mi aspetto inizio stringa, quindi \"", c);
        rhs>>c; //consumo le virgolette di apertura

        string chiave = JSTRING(rhs); // questa è la chiave.

        //mi aspetto quindi obbligatoriamente un fine stringa. prima però controllo EOF
        if (rhs.peek() == -1) error_handler("sono a metà tra key e value, non posso stare in eof", EOF);
        rhs>>c; // prendo quindi effettivamente la " di chiusura, e devo anche consumarla.
        if (c != '"') error_handler("a fine stringa in key, \"", c);

        //ora c'è lo stesso identico discorso con i due punti
        if (rhs.peek() == -1) error_handler("sono a metà tra key e value, non posso stare in eof", EOF);
        rhs>>c; // prendo quindi effettivamente i : e devo anche consumarli.
        if (c != ':') error_handler("sono a metà tra key e value, i :", c);
        
        //ora viene quindi da prendere il valore json. stavolta non devo consumare nulla.
        c = rhs.peek();
        if (controllo_carattere_errore_j(c)) error_handler("un carattere di inizio json come elemento di dict", c);

        json valore = J(rhs);

        //cosa mi aspetto quindi di trovare ora? easy: o un } o un ,
        c = rhs.peek();
        if (c == -1) error_handler("dopo una coppia appesa in dict, non posso trovarmi eof", EOF);
        else if (c != ',' && c != '}') error_handler("dopo una coppia appesa in dict, } o ,", c);
        else if (c != '}'){
            // ovvero se ho una ,
            // nel caso di } non devo fare nulla
            rhs>>c; //consumo la ,
            if ((c=rhs.peek()) == '}') error_handler("non posso avere un elemento didizionario completaente vuoto", '}');
        }

        // ora ho trovato key : value. mi manca solo appenderli effettivamente.
        pair<string, json> da_ins;
        da_ins.first = chiave; da_ins.second = valore;
        j.insert(da_ins);
    }
    //Mi trovo ora che ho appeso tutti gli elementi necessari
    //non devo consumare ] perché se ne occupa il chiamante.
    return j;
}

// JTERM: number  |   true    |   false     |   null
json JTERM(std::istream& rhs){
    // arrivo in JTERM con rhs che punta al primo carattere in modo corretto.
    char c = rhs.peek();
    if (c == -1) error_handler("sto parsando un terminale, non posso trovarmi eof!", EOF);

    json j;

    if (c=='n') {
        char a[] = "null";
        for(short i = 0; i<4; i++){
            rhs>>c;
            if (c != a[i]) error_handler(" la scritta null. in una posizione ", c);
        }
        j.set_null();
    }
    else if ((c >= '0' && c <= '9') || c == '.' || c=='-' || c=='+') {
        double a;
        rhs>>a;
        j.set_number(a);
    }
    else if (c == 'f'){
        char a[] = "false";
        for(short i = 0; i<5; i++){
            rhs>>c;
            if (c != a[i]) error_handler(" la scritta false. in una posizione ", c);
        }
        j.set_bool(false);
    }
    else if (c == 't'){
        char a[] = "true";
        for(short i = 0; i<4; i++){
            rhs>>c;
            if (c != a[i]) error_handler(" la scritta true. in una posizione ", c);
        }
        j.set_bool(true);
    }
    else error_handler("wtf è finito in jterm?", c);
    return j;
}

// JSTRING: µ   |   stringa |    S\"S
string JSTRING (std::istream& rhs){
    //arrivo qui con la " già consumata
    char c = rhs.peek();
    if (c == -1) error_handler("stringa, sono in EOF", EOF);
    
    string s;

    while (c != '"'){
        rhs>>c; //prendo un carattere senza leggerlo.

        if (c == '"') rhs.putback(c); // se trovo una virgoletta di chiusura devo rimetterla in rhs e uscire dal ciclo
        else {
            if (rhs.peek() == -1) error_handler("stringa, sono in EOF", EOF);
            
            s += c; //lo metto nella stringa.

            if (c == '\\'){ //se mi becco davanti un carattere di escape, devo controllare se ho una virgoletta (perché non si chiuda la str)
                rhs>>c; //consumo anche il carattere seguente 
                s += c;
                c = rhs.peek(); // per evitare problemi in guardia, guardo già all'iterazione successiva
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
        for(auto it = rhs.begin_dictionary(); it != rhs.end_dictionary();){
            auto aux = it++;
            if (it != rhs.end_list()) lhs<<"\""<<aux->first<<"\":"<<aux->second<<",";
            else lhs<<"\""<<aux->first<<"\":"<<aux->second;
        }
        lhs<<"}";
    }
    else if (rhs.is_list()){
        lhs<<"[";
        for(auto it = rhs.begin_list(); it != rhs.end_list(); ){
            auto aux = it++;
            if (it == rhs.end_list()) cout<<*aux; 
            else cout<<*aux<<",";
        }
        lhs<<"]";
    }
    else throw json_exception {"qualcosa non va nell'ostream"};
    return lhs;
}

// operatore di input, da dove parte il tutto
std::istream& operator>>(std::istream& lhs, json& rhs){
    try {
        rhs = J(lhs);
    } catch (const json_exception& e) {
        throw e;
    }
    if (lhs.peek() != -1) cout<<"errore: una parte del file è un json corretto, ma il file in sé non lo è, percHé contiene alti caratteri"<<endl;
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

        std::cout<< "JSON letto:\n" << j << std::endl;
    } catch (const json_exception& e) {
        std::cerr << "Errore nel parsing del JSON: " << e.msg << std::endl;
        return 1;
    }

    return 0;
}
