#include<iostream>
#include<string>

using namespace std;

class Dict{
  public:
    Dict();
    ~Dict();

    int& at(string pos);
    void print() const;
    bool operator<=(const Dict& d) const;
    bool operator==(const Dict& d) const;
    Dict& operator+=(const Dict& d);

  private:
    struct Cella{
      string key;
      int val;
      Cella* next;
    };

    typedef Cella* pCella;

    pCella head;


    Cella* append(string str, int val);
    bool presente(pCella lui) const;
    bool chiave_presente(pCella lui) const;
    
};

//Costruttore di default. Non cambiare
Dict::Dict() {
  head = nullptr;
}

//Distruttore: Non cambiare
Dict::~Dict() {
  pCella pc;
  while (head) {
    pc = head;
    head = head->next;
    delete pc;
  }
}

//Stampa gli elementi. Non cambare
void Dict:: print() const {
  cout<<"OUTPUT"<<endl;
  pCella pc = head;

  while (pc) {
    cout<<pc->key<<":"<<pc->val<<endl;
    pc = pc->next;
  }
  cout<<"---"<<endl;
}

Dict::Cella* Dict::append(string str, int val){
    
    pCella nuova = new Cella;
    if (head == nullptr) {
        head = nuova;
        head->val = val;
        head->key = str;
        head->next = nullptr;
        return head;    
    }
    
    pCella aux = head;
    while (aux->next) aux = aux->next;
    aux->next = nuova;
    nuova->next = nullptr;
    nuova->key = str;
    nuova->val = val;
    
    return nuova;
    
}


//Esercizio 1
//Implementare: il metodo at ritorna una reference al
//campo value del nodo con la key uguale a pos se questa esiste
//altrimenti crea un nuovo nodo in coda alla lista che contiene
//come key pos e come valore 0, e ritorna la reference al campo
//value di questo nuovo nodo
int& Dict::at(string pos) {
    if (head == nullptr){
        append(pos, 0);
        return head->val;
    }
    bool trovata = false;
    pCella aux = head;
    while(!trovata && aux){
        if (aux->key != pos) aux=aux->next;
        else trovata = true;
    }
    if (trovata) return aux->val;
    aux = append(pos, 0);
    return aux->val;
}


bool Dict::presente(Dict::pCella lui) const {
    //controllo se una cella è presente in me
    bool trovata = false;
    pCella aux = head;
    
    while(!trovata && aux){
        if (aux->key == lui->key && aux->val == lui->val) trovata = true;
        else aux=aux->next;
    }
    return trovata;
}
//Esercizio 2
//Ritorna true se *this ha un sottoinsieme degli elementi
//di d, cioe' tutte le coppie key,value di *this sono anche
//in Dict (anche in ordine diverso) 
bool Dict::operator<=(const Dict& d) const {
    pCella aux = head;
    if (aux == nullptr) return true;
    if (head == nullptr && aux != nullptr) return false;
    bool contenuti = true;
    while (aux && contenuti){
        if (!d.presente(aux)) contenuti = false;
        aux = aux->next;
    }
    
    return contenuti;
}

//Esercizio 2
//Ritorna true se *this ha gli stessi elementi
//di d anche in ordine diverso
bool Dict::operator==(const Dict& d) const {
    if (head == nullptr && d.head == nullptr) return true;
    if (head == nullptr && d.head != nullptr) return false;
    if (d.head == nullptr && head != nullptr) return false;
    
    bool contenuti = true;
    
    //prima controllo se d è contenuto in this
    pCella aux = d.head;    
    while (aux && contenuti){
        if (!presente(aux)) contenuti = false;
        aux = aux->next;
    }
    
    if (contenuti == false) return false;
    
    //ora faccio il contrario
    aux = head;    
    while (aux && contenuti){
        if (!d.presente(aux)) contenuti = false;
        aux = aux->next;
    }
    
    return contenuti;
}

//Esercizio 3
//Aggiunge tutti gli elementi del parametro formale la cui chiave
//non e' gia' presente in *this. 
//In altre parole, per ogni elemento di d, se la sua key e' gia'
//presente in *this, allora viene ignorato, se invece non e'
//presente, va aggiunto in coda. Al termine si ritorna *this
bool Dict::chiave_presente(Dict::pCella lui) const{
    //controllo se una chiave è presente in me
    bool trovata = false;
    pCella aux = head;
    
    while(!trovata && aux){
        if (aux->key == lui->key) trovata = true;
        else aux=aux->next;
    }
    return trovata;
}


Dict& Dict::operator+=(const Dict& d) {
    pCella aux = d.head;
    if (aux == nullptr) return *this;
    if (head == nullptr) {
        while(aux){
            append(aux->key, aux->val);
            aux = aux->next;
        }
        return *this;
    }
    while (aux){
        if (!chiave_presente(aux)) append(aux->key, aux->val);
        aux = aux->next;
    }
    return *this;
}


void leggi(Dict& s) {
  int nelem;
  string k;
  int v;
  cin>>nelem;
  for (int i=0; i<nelem; i++) {
    cin>>k;
    cin>>v;
    s.at(k)=v;
  }
}

