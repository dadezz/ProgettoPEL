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


//Esercizio 1
//Implementare: il metodo at ritorna una reference al
//campo value del nodo con la key uguale a pos se questa esiste
//altrimenti crea un nuovo nodo in coda alla lista che contiene
//come key pos e come valore 0, e ritorna la reference al campo
//value di questo nuovo nodo
int& Dict::at(string pos) {

}

//Esercizio 2
//Ritorna true se *this ha un sottoinsieme degli elementi
//di d, cioe' tutte le coppie key,value di *this sono anche
//in Dict (anche in ordine diverso) 
bool Dict::operator<=(const Dict& d) const {

}

//Esercizio 2
//Ritorna true se *this ha gli stessi elementi
//di d anche in ordine diverso
bool Dict::operator==(const Dict& d) const {

}

//Esercizio 3
//Aggiunge tutti gli elementi del parametro formale la cui chiave
//non e' gia' presente in *this. 
//In altre parole, per ogni elemento di d, se la sua key e' gia'
//presente in *this, allora viene ignorato, se invece non e'
//presente, va aggiunto in coda. Al termine si ritorna *this
Dict& Dict::operator+=(const Dict& d) {
   
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

