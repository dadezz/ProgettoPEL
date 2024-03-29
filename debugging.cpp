#include <string>
using std::string;
using std::pair;


class json {
    public:
        json();
        json(json const&);
        json(json&&);
        ~json();
        json& operator=(json const&);
        json& operator=(json&&);
        void set_dictionary();
        void insert(std::pair<std::string,json> const&);

    private:
        struct impl;
        impl* pimpl;
};

struct json::impl {
    enum tipologia {nullo, diz}; 
    tipologia Tipo;
    struct Dizionario {
        pair<string, json> info;
        Dizionario* next;
    };
    typedef Dizionario* Dict;
    Dict dict_head, dict_tail;

    void delete_everything();   // riporta un qualsiasi json a tipo "null". ma non dealloca pimpl
    void copy(json const & rhs);   // su un json di tipo null, copia (deep) lo stato di rhs su this
    void cancella_dict();  // dealloca la lista e setta a nullptr i puntatori
    void copia_dict(Dict const & rhs);

};
// default constructor = sets json obj to "null" type
json::json(){
    pimpl = new impl;
    pimpl->Tipo = pimpl->nullo;
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
    cancella_dict();
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
    if (pimpl){
        pimpl->delete_everything(); // svuoto il contenuto di pimpl
        delete pimpl;   // dealloco pimpl
    }
}
//copia (deep) lo stato di rhs su this
void json::impl::copy(json const & rhs){
    Tipo = rhs.pimpl->Tipo;
    copia_dict(rhs.pimpl->dict_head);
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
    if (this != &rhs) {
        pimpl->delete_everything();
        delete pimpl; // Dealloca il pimpl corrente prima di sovrascriverlo
        pimpl = rhs.pimpl;
        rhs.pimpl = nullptr; // Il pimpl dell'oggetto sorgente deve essere lasciato in uno stato valido
    }
    return *this;
}
// move semantics: move constructor
json::json(json&& rhs){
    pimpl = rhs.pimpl;
    rhs.pimpl = nullptr; // Il pimpl dell'oggetto sorgente deve essere lasciato in uno stato valido
}
// rendo this un json(list) con lista vuota
void json::set_dictionary(){
    pimpl->delete_everything();
    pimpl->Tipo = pimpl->diz; 
}
// per semplicità, so come è fatto il main, i puntatori sono null quindi non ho bisogno di un controllo a riguardo
void json::insert(pair<string,json> const& x){
    if (pimpl->dict_head == nullptr) {
        pimpl->dict_head = new impl::Dizionario;
        pimpl->dict_tail = pimpl->dict_head;
    } else {
        pimpl->dict_tail->next = new impl::Dizionario;
        pimpl->dict_tail = pimpl->dict_tail->next;
    }
    
    pimpl->dict_tail->info = x;
    pimpl->dict_tail->next = nullptr;
}
int main() {
//creo un oggetto di tipo json, che inizialmente è completamente vuoto. poi lo setto a dizionario (cambio solo il tipo, rimane vuoto). cerco di popolarlo ma la copia dall'oggetto temporaneo in pair all'oggetto finale (move semantics) va in segfault
    json z;
    z.set_dictionary();
    z.insert({"ciao", json{}});
    return 0;
}
