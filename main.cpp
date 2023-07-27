
int main() {
    std::ifstream file("myfile.json");
    if (!file) {
        std::cerr << "Errore nell'apertura del file." << std::endl;
        return 1;
    }

    json j;
    try {
        file >> j;

        std::cout<< "JSON letto:\n" << j << std::endl;
    } catch (const json_exception& e) {
        std::cerr << "Errore nel parsing del JSON: " << e.msg << std::endl;
        return 1;
    }

    std::cout<<endl<<"testcases:"<<endl;

    try {
    json& y = *(++j.begin_list());
    std::cout << y["quarta chiave"]["a"]<<endl;

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

    z.insert({"ciao", json{}});


    cout<<z<<endl;

    (*(++j.begin_list()))["prima chiave"] = z;

    cout<<j<<endl;
    } catch (const json_exception& e) {
        std::cerr << "Errore nel parsing del JSON: " << e.msg << std::endl;
        return 1;
    }

    try {

    std::ifstream file("large-file.json");
        file >> j;
        std::ofstream fileoo("large-file_letto.json");
        fileoo<<j;
    } catch (const json_exception& e) {
        std::cerr << "Errore nel parsing del JSON: " << e.msg << std::endl;
        return 1;
    }

    return 0;
}

