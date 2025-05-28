#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <limits>
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
        return (dia < 10 ? "0" : "") + std::to_string(dia) + "/" +
               (mes < 10 ? "0" : "") + std::to_string(mes) + "/" +
               std::to_string(ano);
    }

    static Data from_string(const string& str) {
        Data d;
        sscanf(str.c_str(), "%d/%d/%d", &d.dia, &d.mes, &d.ano);
        return d;
    }
};

// Função para comparar datas
int comparar_datas(const Data& a, const Data& b) {
    if (a.ano != b.ano) return a.ano - b.ano;
    if (a.mes != b.mes) return a.mes - b.mes;
    return a.dia - b.dia;
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
            case TipoHardware::COMPUTADOR: return "Computador";
            case TipoHardware::IMPRESSORA: return "Impressora";
            case TipoHardware::SERVIDOR: return "Servidor";
            case TipoHardware::ROTEADOR: return "Roteador";
            case TipoHardware::SWITCH: return "Switch";
            default: return "Outro";
        }
    }

    string to_string() const {
        return "ID: " + std::to_string(id) + " | " + nome + " (" + fabricante + ") | " +
               "Tipo: " + tipo_to_string() + " | Compra: " + dataCompra.to_string() + " | " +
               "Última manutenção: " + ultimaManutencao.to_string() + " | " +
               "Valor: R$" + std::to_string(valorCompra) + " | " +
               (obsoleto ? "OBSOLETO" : "Ativo");
    }
};

// Sistema de inventário com timer
class SistemaInventario {
private:
    vector<Hardware> inventario;
    int proximoId = 1;
    string arquivoDados = "inventario.txt";

    // Timer para medir o tempo das operações
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

public:
    SistemaInventario() {
        carregar_dados();
    }

    ~SistemaInventario() {
        salvar_dados();
    }

    void carregar_dados() {
        ifstream arquivo(arquivoDados);
        if (!arquivo) return;

        string linha;
        while (getline(arquivo, linha)) {
            if (linha.empty()) continue;

            Hardware hw;
            size_t pos = 0;
            string token;

            // ID
            pos = linha.find('|');
            token = linha.substr(0, pos);
            hw.id = stoi(token);
            linha.erase(0, pos + 2);

            // Nome
            pos = linha.find('(');
            hw.nome = linha.substr(0, pos - 1);
            linha.erase(0, pos + 1);

            // Fabricante
            pos = linha.find(')');
            hw.fabricante = linha.substr(0, pos);
            linha.erase(0, pos + 3);

            // Tipo
            pos = linha.find('|');
            token = linha.substr(0, pos - 1);
            if (token == "Computador") hw.tipo = TipoHardware::COMPUTADOR;
            else if (token == "Impressora") hw.tipo = TipoHardware::IMPRESSORA;
            else if (token == "Servidor") hw.tipo = TipoHardware::SERVIDOR;
            else if (token == "Roteador") hw.tipo = TipoHardware::ROTEADOR;
            else if (token == "Switch") hw.tipo = TipoHardware::SWITCH;
            else hw.tipo = TipoHardware::OUTRO;
            linha.erase(0, pos + 2);

            // Data Compra
            pos = linha.find('|');
            hw.dataCompra = Data::from_string(linha.substr(0, pos - 1));
            linha.erase(0, pos + 2);

            // Última Manutenção
            pos = linha.find('|');
            hw.ultimaManutencao = Data::from_string(linha.substr(0, pos - 1));
            linha.erase(0, pos + 2);

            // Valor
            pos = linha.find('|');
            hw.valorCompra = stod(linha.substr(3, pos - 3));
            linha.erase(0, pos + 2);

            // Obsoleto
            hw.obsoleto = (linha.find("OBSOLETO") != string::npos);

            inventario.push_back(hw);
            if (hw.id >= proximoId) proximoId = hw.id + 1;
        }
    }

    void salvar_dados() {
        ofstream arquivo(arquivoDados);
        if (!arquivo) {
            cerr << "Erro ao salvar dados no arquivo!\n";
            return;
        }

        for (const auto& hw : inventario) {
            arquivo << hw.to_string() << "\n";
        }
    }

    void cadastrar_hardware(const string& nome, const string& fabricante, TipoHardware tipo,
                           const Data& dataCompra, double valorCompra, int vidaUtilAnos) {
        medir_tempo("Cadastrar Hardware", [&]() {
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
            for (auto& hw : inventario) {
                if (hw.id == id) {
                    hw.ultimaManutencao = dataManutencao;
                    return true;
                }
            }
            return false;
        });
    }

    void listar_equipamentos() {
        medir_tempo("Listar Equipamentos", [&]() {
            cout << "=== LISTA DE EQUIPAMENTOS (" << inventario.size() << ") ===\n";
            for (const auto& hw : inventario) {
                cout << hw.to_string() << "\n";
            }
        });
    }

    void listar_por_tipo(TipoHardware tipo) {
        medir_tempo("Listar por Tipo", [&]() {
            cout << "=== EQUIPAMENTOS POR TIPO ===\n";
            for (const auto& hw : inventario) {
                if (hw.tipo == tipo) {
                    cout << "ID: " << hw.id << " | " << hw.nome << " (" << hw.fabricante << ")\n";
                }
            }
        });
    }

    void listar_por_data_compra() {
        medir_tempo("Listar por Data de Compra", [&]() {
            vector<Hardware> ordenado = inventario;
            sort(ordenado.begin(), ordenado.end(), [](const Hardware& a, const Hardware& b) {
                return comparar_datas(a.dataCompra, b.dataCompra) < 0;
            });

            cout << "=== EQUIPAMENTOS ORDENADOS POR DATA DE COMPRA ===\n";
            for (const auto& hw : ordenado) {
                cout << hw.to_string() << "\n";
            }
        });
    }

    void listar_por_data_manutencao() {
        medir_tempo("Listar por Data de Manutenção", [&]() {
            vector<Hardware> ordenado = inventario;
            sort(ordenado.begin(), ordenado.end(), [](const Hardware& a, const Hardware& b) {
                return comparar_datas(a.ultimaManutencao, b.ultimaManutencao) < 0;
            });

            cout << "=== EQUIPAMENTOS ORDENADOS POR DATA DE MANUTENÇÃO ===\n";
            for (const auto& hw : ordenado) {
                cout << hw.to_string() << "\n";
            }
        });
    }

    double calcular_depreciacao(const Hardware& hw, const Data& hoje) {
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
            cout << "=== ANÁLISE DE DEPRECIAÇÃO ===\n";
            for (const auto& hw : inventario) {
                double depreciacao = calcular_depreciacao(hw, hoje);
                double valorAtual = hw.valorCompra - depreciacao;

                cout << "ID: " << hw.id << " | " << hw.nome
                     << " | Valor original: R$" << fixed << setprecision(2) << hw.valorCompra
                     << " | Depreciação: R$" << depreciacao
                     << " | Valor atual: R$" << valorAtual << "\n";
            }
        });
    }

    void atualizar_status_obsoleto(const Data& hoje) {
        for (auto& hw : inventario) {
            int anos = hoje.ano - hw.dataCompra.ano;
            if (hoje.mes < hw.dataCompra.mes || (hoje.mes == hw.dataCompra.mes && hoje.dia < hw.dataCompra.dia)) {
                anos--;
            }
            hw.obsoleto = (anos >= hw.vidaUtilAnos);
        }
    }

    void identificar_obsoletos(const Data& hoje) {
        medir_tempo("Identificar Obsoletos", [&]() {
            atualizar_status_obsoleto(hoje);
            cout << "=== EQUIPAMENTOS OBSOLETOS ===\n";
            for (const auto& hw : inventario) {
                if (hw.obsoleto) {
                    cout << "ID: " << hw.id << " | " << hw.nome
                         << " | Compra: " << hw.dataCompra.to_string() << "\n";
                }
            }
        });
    }

    void relatorio_manutencao_pendente(const Data& hoje, int mesesLimite) {
        medir_tempo("Relatório Manutenção Pendente", [&]() {
            cout << "=== EQUIPAMENTOS COM MANUTENÇÃO PENDENTE ===\n";
            for (const auto& hw : inventario) {
                int mesesDesdeManutencao = (hoje.ano - hw.ultimaManutencao.ano) * 12 +
                                          (hoje.mes - hw.ultimaManutencao.mes);
                if (hoje.dia < hw.ultimaManutencao.dia) {
                    mesesDesdeManutencao--;
                }

                if (mesesDesdeManutencao >= mesesLimite) {
                    cout << "ID: " << hw.id << " | " << hw.nome
                         << " | Última manutenção: " << hw.ultimaManutencao.to_string()
                         << " | Meses sem manutenção: " << mesesDesdeManutencao << "\n";
                }
            }
        });
    }
};

// Funções de interface
TipoHardware selecionar_tipo() {
    cout << "Selecione o tipo de hardware:\n";
    cout << "1 - Computador\n2 - Impressora\n3 - Servidor\n";
    cout << "4 - Roteador\n5 - Switch\n6 - Outro\n";

    int op;
    while (true) {
        cin >> op;
        if (cin.fail() || op < 1 || op > 6) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Opção inválida! Tente novamente: ";
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
    cout << mensagem << " (dia mes ano): ";
    while (true) {
        cin >> d.dia >> d.mes >> d.ano;
        if (cin.fail() || d.dia < 1 || d.dia > 31 ||
            d.mes < 1 || d.mes > 12 || d.ano < 1900) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Data inválida! Tente novamente (dia mes ano): ";
            continue;
        }
        break;
    }
    return d;
}

void menu_principal() {
    SistemaInventario sistema;
    Data hoje;

    cout << "=== SISTEMA DE INVENTÁRIO DE HARDWARE ===\n";
    hoje = ler_data("Informe a data atual");

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
                cout << "Opção inválida! Tente novamente: ";
                continue;
            }
            break;
        }

        switch(opcao) {
            case 1: {
                string nome, fabricante;
                cout << "Nome do equipamento: ";
                cin.ignore();
                getline(cin, nome);
                cout << "Fabricante: ";
                getline(cin, fabricante);
                TipoHardware tipo = selecionar_tipo();
                Data dataCompra = ler_data("Data de compra");

                double valor;
                cout << "Valor de compra: ";
                while (!(cin >> valor) || valor < 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Valor inválido! Tente novamente: ";
                }

                int vidaUtil;
                cout << "Vida útil em anos: ";
                while (!(cin >> vidaUtil) || vidaUtil < 1) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Valor inválido! Tente novamente: ";
                }

                sistema.cadastrar_hardware(nome, fabricante, tipo, dataCompra, valor, vidaUtil);
                break;
            }
            case 2: {
                int id;
                cout << "ID do equipamento: ";
                while (!(cin >> id) || id < 1) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "ID inválido! Tente novamente: ";
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
                int meses;
                cout << "Informe o limite de meses sem manutenção: ";
                while (!(cin >> meses) || meses < 1) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Valor inválido! Tente novamente: ";
                }
                sistema.relatorio_manutencao_pendente(hoje, meses);
                break;
            }
            case 0:
                cout << "Salvando dados e saindo...\n";
                break;
        }
    } while (opcao != 0);
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    menu_principal();
    return 0;
}