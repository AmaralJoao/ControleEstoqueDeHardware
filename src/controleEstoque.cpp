#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <limits>
#include <sstream>
#include <functional>
#include <vector> 
#include <windows.h>

using namespace std;
using namespace std::chrono;

// Enum para tipos de hardware
enum class TipoHardware {
    COMPUTADOR,
    IMPRESSORA,
    SERVIDOR,
    ROTEADOR,
    SWITCH,
    OUTRO
};

// Estrutura para representar uma data
struct Data {
    int dia;
    int mes;
    int ano;

    string to_string() const {
        ostringstream oss;
        oss << setfill('0') << setw(2) << dia << "/" 
            << setw(2) << mes << "/" << setw(4) << ano;
        return oss.str();
    }

    static Data from_string(const string& str) {
        Data d;
        char sep;
        istringstream iss(str);
        iss >> d.dia >> sep >> d.mes >> sep >> d.ano;
        if (iss.fail() || sep != '/') {
            throw runtime_error("Formato de data inválido");
        }
        return d;
    }

    bool operator<(const Data& outra) const {
        if (ano != outra.ano) return ano < outra.ano;
        if (mes != outra.mes) return mes < outra.mes;
        return dia < outra.dia;
    }
};

// Função para obter a data atual do sistema
Data obter_data_atual() {
    time_t agora = time(nullptr);
    tm* tempo_local = localtime(&agora);
    
    Data atual;
    atual.dia = tempo_local->tm_mday;
    atual.mes = tempo_local->tm_mon + 1; // tm_mon vai de 0-11
    atual.ano = tempo_local->tm_year + 1900; // tm_year é anos desde 1900
    
    return atual;
}

// Estrutura para representar um equipamento de hardware
struct Hardware {
    int id;
    string nome;
    string fabricante;
    TipoHardware tipo;
    Data dataCompra;
    double valorCompra;
    int vidaUtilAnos;
    Data ultimaManutencao;
    bool obsoleto;

    string tipo_to_string() const {
        switch(tipo) {
            case TipoHardware::COMPUTADOR: return "COMPUTADOR";
            case TipoHardware::IMPRESSORA: return "IMPRESSORA";
            case TipoHardware::SERVIDOR: return "SERVIDOR";
            case TipoHardware::ROTEADOR: return "ROTEADOR";
            case TipoHardware::SWITCH: return "SWITCH";
            default: return "OUTRO";
        }
    }

    static TipoHardware string_to_tipo(const string& str) {
        if (str == "COMPUTADOR") return TipoHardware::COMPUTADOR;
        if (str == "IMPRESSORA") return TipoHardware::IMPRESSORA;
        if (str == "SERVIDOR") return TipoHardware::SERVIDOR;
        if (str == "ROTEADOR") return TipoHardware::ROTEADOR;
        if (str == "SWITCH") return TipoHardware::SWITCH;
        return TipoHardware::OUTRO;
    }

    string to_csv() const {
        ostringstream oss;
        oss << id << ";"
            << nome << ";"
            << fabricante << ";"
            << tipo_to_string() << ";"
            << dataCompra.to_string() << ";"
            << fixed << setprecision(2) << valorCompra << ";"
            << vidaUtilAnos << ";"
            << ultimaManutencao.to_string() << ";"
            << (obsoleto ? "1" : "0");
        return oss.str();
    }

    static Hardware from_csv(const string& linha) {
        vector<string> campos;
        string campo;
        istringstream iss(linha);
        
        while (getline(iss, campo, ';')) {
            campos.push_back(campo);
        }

        if (campos.size() != 9) {
            throw runtime_error("Formato de linha inválido");
        }

        Hardware hw;
        try {
            hw.id = stoi(campos[0]);
            hw.nome = campos[1];
            hw.fabricante = campos[2];
            hw.tipo = string_to_tipo(campos[3]);
            hw.dataCompra = Data::from_string(campos[4]);
            hw.valorCompra = stod(campos[5]);
            hw.vidaUtilAnos = stoi(campos[6]);
            hw.ultimaManutencao = Data::from_string(campos[7]);
            hw.obsoleto = (campos[8] == "1");
        } catch (const exception& e) {
            throw runtime_error("Erro ao converter dados: " + string(e.what()));
        }

        return hw;
    }

    string to_string() const {
        ostringstream oss;
        oss << "ID: " << id << " | " << nome << " (" << fabricante << ") | "
            << "Tipo: " << tipo_to_string() << " | Compra: " << dataCompra.to_string() << " | "
            << "Última manutenção: " << ultimaManutencao.to_string() << " | "
            << "Valor: R$" << fixed << setprecision(2) << valorCompra << " | "
            << "Vida útil: " << vidaUtilAnos << " anos | "
            << (obsoleto ? "OBSOLETO" : "Ativo");
        return oss.str();
    }
};

// Nó para lista encadeada
struct Node {
    Hardware data;
    Node* next;
    
    Node(const Hardware& hw) : data(hw), next(nullptr) {}
};

// Implementação de lista encadeada
class LinkedList {
private:
    Node* head;
    Node* tail;
    int size;

    void swap_nodes(Node* prev_a, Node* a, Node* prev_b, Node* b) {
        if (a == b) return;
        
        // Atualiza os ponteiros next dos nós vizinhos
        if (prev_a) prev_a->next = b;
        else head = b;
        
        if (prev_b) prev_b->next = a;
        else head = a;
        
        // Atualiza os ponteiros next dos nós trocados
        Node* temp = a->next;
        a->next = b->next;
        b->next = temp;
        
        // Atualiza tail se necessário
        if (tail == a) tail = b;
        else if (tail == b) tail = a;
    }

public:
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}
    
    ~LinkedList() {
        clear();
    }

    void push_back(const Hardware& hw) {
        Node* newNode = new Node(hw);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }

    void clear() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = tail = nullptr;
        size = 0;
    }

    Node* get_head() const { return head; }
    int get_size() const { return size; }

    // Algoritmo de ordenação Bubble Sort
    void bubble_sort(function<bool(const Hardware&, const Hardware&)> compare) {
        if (size < 2) return;
        
        bool swapped;
        Node *current, *prev = nullptr;
        
        do {
            swapped = false;
            current = head;
            prev = nullptr;
            
            while (current->next) {
                if (!compare(current->data, current->next->data)) {
                    swap_nodes(prev, current, current, current->next);
                    swapped = true;
                } else {
                    prev = current;
                    current = current->next;
                }
            }
        } while (swapped);
    }

    // Algoritmo de ordenação Insertion Sort
    void insertion_sort(function<bool(const Hardware&, const Hardware&)> compare) {
        if (size < 2) return;
        
        Node* sorted = nullptr;
        Node* current = head;
        
        while (current) {
            Node* next = current->next;
            
            if (!sorted || compare(current->data, sorted->data)) {
                current->next = sorted;
                sorted = current;
            } else {
                Node* temp = sorted;
                while (temp->next && !compare(current->data, temp->next->data)) {
                    temp = temp->next;
                }
                current->next = temp->next;
                temp->next = current;
            }
            
            current = next;
        }
        
        head = sorted;
        // Atualiza tail
        tail = sorted;
        while (tail && tail->next) tail = tail->next;
    }
};

// Sistema de inventário com lista encadeada
class SistemaInventario {
private:
    LinkedList inventario;
    int proximoId = 1;
    const string arquivoDados = "inventario.csv";

    template<typename Func>
    void medir_tempo(const string& nome_operacao, Func func) {
        auto inicio = high_resolution_clock::now();
        func();
        auto fim = high_resolution_clock::now();
        auto duracao = duration_cast<milliseconds>(fim - inicio);
        cout << "Operação '" << nome_operacao << "' concluída em " 
             << duracao.count() << " ms\n";
    }

    template<typename Func, typename... Args>
    auto medir_tempo_com_retorno(const string& nome_operacao, Func func, Args... args) {
        auto inicio = high_resolution_clock::now();
        auto resultado = func(args...);
        auto fim = high_resolution_clock::now();
        auto duracao = duration_cast<milliseconds>(fim - inicio);
        cout << "Operação '" << nome_operacao << "' concluída em " 
             << duracao.count() << " ms\n";
        return resultado;
    }

    // Método adicionado para copiar a lista
    LinkedList copiar_lista() const {
        LinkedList nova_lista;
        Node* current = inventario.get_head();
        while (current) {
            nova_lista.push_back(current->data);
            current = current->next;
        }
        return nova_lista;
    }

public:
    SistemaInventario() {
        carregar_dados();
        // Atualiza o próximo ID disponível
        int max_id = 0;
        Node* current = inventario.get_head();
        while (current) {
            if (current->data.id > max_id) {
                max_id = current->data.id;
            }
            current = current->next;
        }
        proximoId = max_id + 1;
    }

    ~SistemaInventario() {
        salvar_dados();
    }

    void carregar_dados() {
        ifstream arquivo(arquivoDados);
        if (!arquivo) {
            cout << "Arquivo de dados não encontrado. Criando novo inventário.\n";
            return;
        }

        cout << "Carregando dados do arquivo...\n";
        string linha;
        getline(arquivo, linha); // Pula cabeçalho

        int contador = 0;
        while (getline(arquivo, linha)) {
            if (linha.empty()) continue;
            try {
                Hardware hw = Hardware::from_csv(linha);
                inventario.push_back(hw);
                contador++;
            } catch (const exception& e) {
                cerr << "Erro ao carregar linha: " << linha << " - " << e.what() << "\n";
            }
        }
        cout << "Dados carregados com sucesso. " << contador << " itens encontrados.\n";
    }

    void salvar_dados() {
        ofstream arquivo(arquivoDados);
        if (!arquivo) {
            cerr << "Erro ao abrir arquivo para salvar dados!\n";
            return;
        }

        // Escreve cabeçalho
        arquivo << "ID;Nome;Fabricante;Tipo;DataCompra;Valor;VidaUtil;UltimaManutencao;Obsoleto\n";

        int contador = 0;
        Node* current = inventario.get_head();
        while (current) {
            arquivo << current->data.to_csv() << "\n";
            current = current->next;
            contador++;
        }
        cout << "Dados salvos com sucesso (" << contador << " itens) no arquivo " << arquivoDados << "\n";
    }

    void cadastrar_hardware(const string& nome, const string& fabricante, TipoHardware tipo,
                           const Data& dataCompra, double valorCompra, int vidaUtilAnos) {
        medir_tempo("Cadastrar Hardware", [&]() {
            if (nome.empty() || fabricante.empty()) {
                throw invalid_argument("Nome e fabricante não podem estar vazios");
            }
            if (valorCompra <= 0 || vidaUtilAnos <= 0) {
                throw invalid_argument("Valor e vida útil devem ser positivos");
            }

            Hardware hw;
            hw.id = proximoId++;
            hw.nome = nome;
            hw.fabricante = fabricante;
            hw.tipo = tipo;
            hw.dataCompra = dataCompra;
            hw.valorCompra = valorCompra;
            hw.vidaUtilAnos = vidaUtilAnos;
            hw.ultimaManutencao = dataCompra;
            hw.obsoleto = false;

            inventario.push_back(hw);
            cout << "Hardware cadastrado com ID: " << hw.id << "\n";
        });
    }

    bool registrar_manutencao(int id, const Data& dataManutencao) {
        return medir_tempo_com_retorno("Registrar Manutenção", [&]() {
            Node* current = inventario.get_head();
            while (current) {
                if (current->data.id == id) {
                    current->data.ultimaManutencao = dataManutencao;
                    return true;
                }
                current = current->next;
            }
            return false;
        });
    }

    void listar_equipamentos() {
        medir_tempo("Listar Equipamentos", [&]() {
            cout << "=== LISTA DE EQUIPAMENTOS (" << inventario.get_size() << ") ===\n";
            Node* current = inventario.get_head();
            while (current) {
                cout << current->data.to_string() << "\n";
                current = current->next;
            }
        });
    }

    void listar_por_tipo(TipoHardware tipo) {
        medir_tempo("Listar por Tipo", [&]() {
            cout << "=== EQUIPAMENTOS POR TIPO (" << Hardware().tipo_to_string() << ") ===\n";
            int contador = 0;
            Node* current = inventario.get_head();
            while (current) {
                if (current->data.tipo == tipo) {
                    cout << current->data.to_string() << "\n";
                    contador++;
                }
                current = current->next;
            }
            cout << "Total encontrado: " << contador << " equipamentos\n";
        });
    }

    void listar_por_data_compra() {
        medir_tempo("Listar por Data de Compra", [&]() {
            LinkedList temp = copiar_lista(); // Usando o novo método de cópia
            
            // Usa insertion sort para ordenar por data de compra
            temp.insertion_sort([](const Hardware& a, const Hardware& b) {
                return a.dataCompra < b.dataCompra;
            });
            
            cout << "=== EQUIPAMENTOS ORDENADOS POR DATA DE COMPRA ===\n";
            Node* current = temp.get_head();
            while (current) {
                cout << current->data.to_string() << "\n";
                current = current->next;
            }
        });
    }

    void listar_por_data_manutencao() {
        medir_tempo("Listar por Data de Manutenção", [&]() {
            LinkedList temp = copiar_lista(); // Usando o novo método de cópia
            
            // Usa bubble sort para ordenar por data de manutenção
            temp.bubble_sort([](const Hardware& a, const Hardware& b) {
                return a.ultimaManutencao < b.ultimaManutencao;
            });
            
            cout << "=== EQUIPAMENTOS ORDENADOS POR DATA DE MANUTENÇÃO ===\n";
            Node* current = temp.get_head();
            while (current) {
                cout << current->data.to_string() << "\n";
                current = current->next;
            }
        });
    }

    double calcular_depreciacao(const Hardware& hw, const Data& hoje) const {
        int anos = hoje.ano - hw.dataCompra.ano;
        if (hoje.mes < hw.dataCompra.mes || (hoje.mes == hw.dataCompra.mes && hoje.dia < hw.dataCompra.dia)) {
            anos--;
        }
        
        if (anos <= 0) return 0.0;
        if (anos >= hw.vidaUtilAnos) return hw.valorCompra;
        
        return (hw.valorCompra / hw.vidaUtilAnos) * anos;
    }

    void mostrar_analise_depreciacao(const Data& hoje) {
        medir_tempo("Análise de Depreciação", [&]() {
            cout << "=== ANÁLISE DE DEPRECIAÇÃO (Data base: " << hoje.to_string() << ") ===\n";
            double total_original = 0, total_depreciado = 0;
            
            Node* current = inventario.get_head();
            while (current) {
                double depreciacao = calcular_depreciacao(current->data, hoje);
                double valorAtual = current->data.valorCompra - depreciacao;
                
                cout << "ID: " << current->data.id << " | " << current->data.nome 
                     << " | Valor original: R$" << fixed << setprecision(2) << current->data.valorCompra
                     << " | Depreciação: R$" << depreciacao
                     << " | Valor atual: R$" << valorAtual << "\n";
                
                total_original += current->data.valorCompra;
                total_depreciado += depreciacao;
                current = current->next;
            }
            
            cout << "----------------------------------------------------------------\n";
            cout << "TOTAL | Valor original: R$" << total_original 
                 << " | Depreciação total: R$" << total_depreciado
                 << " | Valor atual total: R$" << (total_original - total_depreciado) << "\n";
        });
    }

    void atualizar_status_obsoleto(const Data& hoje) {
        Node* current = inventario.get_head();
        while (current) {
            int anos = hoje.ano - current->data.dataCompra.ano;
            if (hoje.mes < current->data.dataCompra.mes || 
                (hoje.mes == current->data.dataCompra.mes && hoje.dia < current->data.dataCompra.dia)) {
                anos--;
            }
            current->data.obsoleto = (anos >= current->data.vidaUtilAnos);
            current = current->next;
        }
    }

    void identificar_obsoletos(const Data& hoje) {
        medir_tempo("Identificar Obsoletos", [&]() {
            atualizar_status_obsoleto(hoje);
            cout << "=== EQUIPAMENTOS OBSOLETOS (Data base: " << hoje.to_string() << ") ===\n";
            int contador = 0;
            Node* current = inventario.get_head();
            while (current) {
                if (current->data.obsoleto) {
                    cout << "ID: " << current->data.id << " | " << current->data.nome 
                         << " | Compra: " << current->data.dataCompra.to_string()
                         << " | Vida útil: " << current->data.vidaUtilAnos << " anos\n";
                    contador++;
                }
                current = current->next;
            }
            cout << "Total de obsoletos: " << contador << "\n";
        });
    }

    void relatorio_manutencao_pendente(const Data& hoje, int mesesLimite) {
        medir_tempo("Relatório Manutenção Pendente", [&]() {
            cout << "=== EQUIPAMENTOS COM MANUTENÇÃO PENDENTE (> " << mesesLimite 
                 << " meses, Data base: " << hoje.to_string() << ") ===\n";
            int contador = 0;
            Node* current = inventario.get_head();
            while (current) {
                int mesesDesdeManutencao = (hoje.ano - current->data.ultimaManutencao.ano) * 12 + 
                                          (hoje.mes - current->data.ultimaManutencao.mes);
                if (hoje.dia < current->data.ultimaManutencao.dia) {
                    mesesDesdeManutencao--;
                }
                
                if (mesesDesdeManutencao >= mesesLimite) {
                    cout << "ID: " << current->data.id << " | " << current->data.nome
                         << " | Última manutenção: " << current->data.ultimaManutencao.to_string()
                         << " | Meses sem manutenção: " << mesesDesdeManutencao << "\n";
                    contador++;
                }
                current = current->next;
            }
            cout << "Total com manutenção pendente: " << contador << "\n";
        });
    }
};

// Funções de interface
TipoHardware selecionar_tipo() {
    cout << "\nSelecione o tipo de hardware:\n";
    cout << "1 - Computador\n2 - Impressora\n3 - Servidor\n";
    cout << "4 - Roteador\n5 - Switch\n6 - Outro\n";
    cout << "Opção: ";
    
    int op;
    while (true) {
        cin >> op;
        if (cin.fail() || op < 1 || op > 6) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Opção inválida! Tente novamente (1-6): ";
            continue;
        }
        break;
    }

    switch(op) {
        case 1: return TipoHardware::COMPUTADOR;
        case 2: return TipoHardware::IMPRESSORA;
        case 3: return TipoHardware::SERVIDOR;
        case 4: return TipoHardware::ROTEADOR;
        case 5: return TipoHardware::SWITCH;
        default: return TipoHardware::OUTRO;
    }
}

Data ler_data(const string& mensagem) {
    Data d;
    cout << mensagem << " (DD/MM/AAAA): ";
    char sep;
    while (true) {
        cin >> d.dia >> sep >> d.mes >> sep >> d.ano;
        if (cin.fail() || sep != '/' || d.dia < 1 || d.dia > 31 || 
            d.mes < 1 || d.mes > 12 || d.ano < 1900) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Data inválida! Use o formato DD/MM/AAAA: ";
            continue;
        }
        break;
    }
    return d;
}

void menu_principal() {
    SistemaInventario sistema;
    Data hoje = obter_data_atual();  // Usando a nova função para obter data atual

    cout << "\n=== SISTEMA DE INVENTÁRIO DE HARDWARE ===\n";
    cout << "Data atual do sistema: " << hoje.to_string() << "\n";

    int opcao;
    do {
        cout << "\n=== MENU PRINCIPAL ===\n";
        cout << "1 - Cadastrar novo hardware\n";
        cout << "2 - Registrar manutenção\n";
        cout << "3 - Listar todos os equipamentos\n";
        cout << "4 - Listar equipamentos por tipo\n";
        cout << "5 - Listar equipamentos ordenados por data de compra\n";
        cout << "6 - Listar equipamentos ordenados por data de manutenção\n";
        cout << "7 - Mostrar análise de depreciação\n";
        cout << "8 - Identificar equipamentos obsoletos\n";
        cout << "9 - Relatório de manutenção pendente\n";
        cout << "0 - Sair\n";
        cout << "Opção: ";

        while (true) {
            cin >> opcao;
            if (cin.fail() || opcao < 0 || opcao > 9) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Opção inválida! Tente novamente (0-9): ";
                continue;
            }
            break;
        }

        try {
            switch(opcao) {
                case 1: {
                    string nome, fabricante;
                    cout << "\n--- CADASTRO DE HARDWARE ---\n";
                    cout << "Nome do equipamento: ";
                    cin.ignore();
                    getline(cin, nome);
                    cout << "Fabricante: ";
                    getline(cin, fabricante);
                    TipoHardware tipo = selecionar_tipo();
                    Data dataCompra = ler_data("Data de compra");
                    
                    double valor;
                    cout << "Valor de compra (R$): ";
                    while (!(cin >> valor) || valor <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Valor inválido! Digite um valor positivo: ";
                    }
                    
                    int vidaUtil;
                    cout << "Vida útil em anos: ";
                    while (!(cin >> vidaUtil) || vidaUtil <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Valor inválido! Digite um número positivo: ";
                    }
                    
                    sistema.cadastrar_hardware(nome, fabricante, tipo, dataCompra, valor, vidaUtil);
                    break;
                }
                case 2: {
                    cout << "\n--- REGISTRAR MANUTENÇÃO ---\n";
                    int id;
                    cout << "ID do equipamento: ";
                    while (!(cin >> id) || id <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "ID inválido! Digite um número positivo: ";
                    }
                    Data dataManutencao = ler_data("Data da manutenção");
                    if (sistema.registrar_manutencao(id, dataManutencao)) {
                        cout << "Manutenção registrada com sucesso!\n";
                    } else {
                        cout << "Equipamento não encontrado!\n";
                    }
                    break;
                }
                case 3:
                    sistema.listar_equipamentos();
                    break;
                case 4: {
                    TipoHardware tipo = selecionar_tipo();
                    sistema.listar_por_tipo(tipo);
                    break;
                }
                case 5:
                    sistema.listar_por_data_compra();
                    break;
                case 6:
                    sistema.listar_por_data_manutencao();
                    break;
                case 7:
                    sistema.mostrar_analise_depreciacao(hoje);
                    break;
                case 8:
                    sistema.identificar_obsoletos(hoje);
                    break;
                case 9: {
                    cout << "\n--- RELATÓRIO DE MANUTENÇÃO PENDENTE ---\n";
                    int meses;
                    cout << "Informe o limite de meses sem manutenção: ";
                    while (!(cin >> meses) || meses <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Valor inválido! Digite um número positivo: ";
                    }
                    sistema.relatorio_manutencao_pendente(hoje, meses);
                    break;
                }
                case 0:
                    cout << "\nSalvando dados e saindo...\n";
                    break;
            }
        } catch (const exception& e) {
            cerr << "\nErro: " << e.what() << "\n";
        }
    } while (opcao != 0);
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    try {
        menu_principal();
    } catch (const exception& e) {
        cerr << "\nErro fatal: " << e.what() << "\n";
        return 1;
    }
    return 0;
}