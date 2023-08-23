#include "json.cpp"

int main() {
    
    std::cout<<"\n====================\nPRIMO TESTCASE, LETTURA myfile.json"<<std::endl;
    std::cout<<"====================\n"<<std::endl;
    
    std::ifstream file("myfile.json");
    if (!file) {
        std::cerr << "Errore nell'apertura del file." << std::endl;
        return 1;
    }

    json j;
    try {
        file >> j;
        
        std::cout<<"\n====================\nSECONDO TESTCASE, cout di ciò che ho letto di myfile.json"<<std::endl;
        std::cout<<"====================\n"<<std::endl;
        
        std::cout<< "JSON letto:\n" << j << std::endl;
    } catch (const json_exception& e) {
        std::cerr << "Errore nel parsing del JSON: " << e.msg << std::endl;
        return 1;
    }

    std::cout<<"\n====================\nTERZO TESTCASE, gioco con iteratori e []"<<std::endl;
    std::cout<<"====================\n"<<std::endl;

    try {
    json& y = *(++j.begin_list());
    std::cout << y["quarta chiave"]["a"]<<endl;

    std::cout<<"\n====================\nQUARTO TESTCASE, gioco con set()"<<std::endl;
    std::cout<<"====================\n"<<std::endl;
    json z;
    z.set_dictionary();
    cout<<z<<endl;
    json aux;
    aux.set_number(5);
    cout<<aux<<endl;
    cout<<z<<endl;
    aux.set_number(6);
    cout<<aux<<endl;
   

    // aux è una variabile di tipo json (in questo caso specifico, un numero)

    pair<string,json> p;
    p.first = string{"d"}; 
    p.second = aux;
    z.insert(p);
    
    std::cout<<"\n====================\nquarto e mezzo TESTCASE, move semantics"<<std::endl;
    std::cout<<"====================\n"<<std::endl;

    z.insert({"ciao", json{}});

    std::cout<<"\n====================\nQUINTO TESTCASE, move semantics"<<std::endl;
    std::cout<<"====================\n"<<std::endl;

    json u = z;


    cout<<z<<endl;

    (*(++j.begin_list()))["prima chiave"] = z;

    cout<<j<<endl;
    } catch (const json_exception& e) {
        std::cerr << "Errore nel parsing del JSON: " << e.msg << std::endl;
        return 1;
    }

    try {

    std::cout<<"====================\nSESTO TESTCASE, file grande, output come file"<<std::endl;
    std::cout<<"====================\n"<<std::endl;

    std::ifstream file("json2.json");
        file >> j;
        std::ofstream fileoo("json2_letto.json");
        fileoo<<j;
    } catch (const json_exception& e) {
        std::cerr << "Errore nel parsing del JSON: " << e.msg << std::endl;
        return 1;
    }

    return 0;
}

