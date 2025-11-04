/*
 * Sistema de Gestion de Procesos
 * Proyecto Universitario - Estructura de Datos
 * 
 * Implementamos tres estructuras de datos:
 * 1. Lista enlazada para gestionar procesos
 * 2. Cola de prioridad para planificar ejecucion
 * 3. Pila para administrar memoria
 * 
 * Todas las estructuras estan implementadas desde cero.
 * Version: 2.0 - Solucion errores compilacion
 */

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <limits>
using namespace std;

// ========== ESTRUCTURAS BASICAS ==========

struct Proceso {
    int id;
    string nombre;
    int prioridad;
    int memoria;
    Proceso* siguiente;
};

struct NodoCola {
    Proceso proceso;
    NodoCola* siguiente;
};

struct NodoPila {
    int idProceso;
    int memoriaUsada;
    NodoPila* siguiente;
};

// ========== CLASES PARA ESTRUCTURAS DE DATOS ==========

class ListaProcesos {
private:
    Proceso* cabeza;
    int ultimoId;

public:
    ListaProcesos() : cabeza(nullptr), ultimoId(0) {}
    
    ~ListaProcesos() {
        limpiarLista();
    }

    void insertarProceso(string nombre, int prioridad, int memoria) {
        Proceso* nuevo = new Proceso;
        nuevo->id = ++ultimoId;
        nuevo->nombre = nombre;
        nuevo->prioridad = prioridad;
        nuevo->memoria = memoria;
        nuevo->siguiente = nullptr;
        
        if(cabeza == nullptr) {
            cabeza = nuevo;
        } else {
            Proceso* actual = cabeza;
            while(actual->siguiente != nullptr) {
                actual = actual->siguiente;
            }
            actual->siguiente = nuevo;
        }
        cout << "Proceso '" << nombre << "' creado con ID: " << ultimoId << endl;
    }

    bool eliminarProceso(int id) {
        if(cabeza == nullptr) {
            cout << "No hay procesos para eliminar." << endl;
            return false;
        }
        
        Proceso* actual = cabeza;
        Proceso* anterior = nullptr;
        
        while(actual != nullptr && actual->id != id) {
            anterior = actual;
            actual = actual->siguiente;
        }
        
        if(actual == nullptr) {
            cout << "Proceso con ID " << id << " no encontrado." << endl;
            return false;
        }
        
        if(anterior == nullptr) {
            cabeza = actual->siguiente;
        } else {
            anterior->siguiente = actual->siguiente;
        }
        
        cout << "Proceso '" << actual->nombre << "' eliminado correctamente." << endl;
        delete actual;
        return true;
    }

    Proceso* buscarProceso(int id) {
        Proceso* actual = cabeza;
        while(actual != nullptr) {
            if(actual->id == id) {
                return actual;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    bool modificarPrioridad(int id, int nuevaPrioridad) {
        Proceso* proceso = buscarProceso(id);
        if(proceso != nullptr) {
            cout << "Prioridad de '" << proceso->nombre << "' cambiada: " 
                 << proceso->prioridad << " a " << nuevaPrioridad << endl;
            proceso->prioridad = nuevaPrioridad;
            return true;
        }
        cout << "Proceso con ID " << id << " no encontrado." << endl;
        return false;
    }

    void mostrarProcesos() {
        if(cabeza == nullptr) {
            cout << "No hay procesos registrados." << endl;
            return;
        }
        
        cout << "\nLISTA DE PROCESOS REGISTRADOS:" << endl;
        cout << "-----------------------------------" << endl;
        Proceso* actual = cabeza;
        while(actual != nullptr) {
            cout << "ID: " << actual->id << " | " 
                 << "Nombre: " << actual->nombre << " | "
                 << "Prioridad: " << actual->prioridad << " | "
                 << "Memoria: " << actual->memoria << " MB" << endl;
            actual = actual->siguiente;
        }
        cout << "-----------------------------------" << endl;
    }

    Proceso* getCabeza() { return cabeza; }
    int getUltimoId() { return ultimoId; }
    void setUltimoId(int id) { ultimoId = id; }

private:
    void limpiarLista() {
        Proceso* actual = cabeza;
        while(actual != nullptr) {
            Proceso* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
        cabeza = nullptr;
    }
};

class ColaCPU {
private:
    NodoCola* frente;
    NodoCola* final;

public:
    ColaCPU() : frente(nullptr), final(nullptr) {}
    
    ~ColaCPU() {
        limpiarCola();
    }

    void encolarProceso(Proceso proceso) {
        NodoCola* nuevo = new NodoCola;
        nuevo->proceso = proceso;
        nuevo->siguiente = nullptr;
        
        if(frente == nullptr) {
            frente = nuevo;
            final = nuevo;
        } else {
            NodoCola* actual = frente;
            NodoCola* anterior = nullptr;
            
            while(actual != nullptr && actual->proceso.prioridad <= proceso.prioridad) {
                anterior = actual;
                actual = actual->siguiente;
            }
            
            if(anterior == nullptr) {
                nuevo->siguiente = frente;
                frente = nuevo;
            } else if(actual == nullptr) {
                final->siguiente = nuevo;
                final = nuevo;
            } else {
                anterior->siguiente = nuevo;
                nuevo->siguiente = actual;
            }
        }
    }

    bool desencolarProceso() {
        if(frente == nullptr) {
            cout << "No hay procesos en la cola de ejecucion." << endl;
            return false;
        }
        
        NodoCola* temp = frente;
        cout << "Ejecutando: " << temp->proceso.nombre 
             << " (Prioridad: " << temp->proceso.prioridad << ")" << endl;
        
        frente = frente->siguiente;
        if(frente == nullptr) {
            final = nullptr;
        }
        
        delete temp;
        return true;
    }

    void mostrarColaEjecucion() {
        if(frente == nullptr) {
            cout << "Cola de ejecucion vacia." << endl;
            return;
        }
        
        cout << "\nCOLA DE EJECUCION (orden de prioridad):" << endl;
        cout << "-----------------------------------" << endl;
        NodoCola* actual = frente;
        int posicion = 1;
        while(actual != nullptr) {
            cout << posicion << ". " << actual->proceso.nombre 
                 << " (Prioridad: " << actual->proceso.prioridad << ")" << endl;
            actual = actual->siguiente;
            posicion++;
        }
        cout << "-----------------------------------" << endl;
    }

private:
    void limpiarCola() {
        while(frente != nullptr) {
            NodoCola* temp = frente;
            frente = frente->siguiente;
            delete temp;
        }
        final = nullptr;
    }
};

class PilaMemoria {
private:
    NodoPila* tope;
    int memoriaTotal;
    int memoriaUsada;

public:
    PilaMemoria(int totalMemoria = 1024) : tope(nullptr), memoriaTotal(totalMemoria), memoriaUsada(0) {}
    
    ~PilaMemoria() {
        limpiarPila();
    }

    bool asignarMemoria(int idProceso, int memoria) {
        if(memoriaUsada + memoria > memoriaTotal) {
            cout << "Memoria insuficiente. Necesita: " << memoria 
                 << " MB, Disponible: " << (memoriaTotal - memoriaUsada) << " MB" << endl;
            return false;
        }
        
        NodoPila* nuevo = new NodoPila;
        nuevo->idProceso = idProceso;
        nuevo->memoriaUsada = memoria;
        nuevo->siguiente = tope;
        tope = nuevo;
        
        memoriaUsada += memoria;
        cout << "Memoria asignada: " << memoria << " MB al proceso ID: " << idProceso << endl;
        return true;
    }

    bool liberarBloqueMemoria() {
        if(tope == nullptr) {
            cout << "No hay memoria asignada para liberar." << endl;
            return false;
        }
        
        NodoPila* temp = tope;
        cout << "Memoria liberada: " << temp->memoriaUsada 
             << " MB del proceso ID: " << temp->idProceso << endl;
        
        memoriaUsada -= temp->memoriaUsada;
        tope = tope->siguiente;
        delete temp;
        return true;
    }

    void mostrarEstadoMemoria() {
        cout << "\nESTADO DE MEMORIA:" << endl;
        cout << "-----------------------------------" << endl;
        cout << "Memoria total: " << memoriaTotal << " MB" << endl;
        cout << "Memoria usada: " << memoriaUsada << " MB" << endl;
        cout << "Memoria disponible: " << (memoriaTotal - memoriaUsada) << " MB" << endl;
        
        if(tope != nullptr) {
            cout << "\nBloques de memoria asignados:" << endl;
            NodoPila* actual = tope;
            while(actual != nullptr) {
                cout << "Proceso ID: " << actual->idProceso 
                     << " - Memoria: " << actual->memoriaUsada << " MB" << endl;
                actual = actual->siguiente;
            }
        }
        cout << "-----------------------------------" << endl;
    }

    int getMemoriaDisponible() { return memoriaTotal - memoriaUsada; }

private:
    void limpiarPila() {
        while(tope != nullptr) {
            NodoPila* temp = tope;
            tope = tope->siguiente;
            delete temp;
        }
        memoriaUsada = 0;
    }
};

// ========== VARIABLES GLOBALES ==========

ListaProcesos listaProcesos;
ColaCPU colaCPU;
PilaMemoria pilaMemoria(1024);

// ========== FUNCIONES DE VALIDACION ==========

int leerEntero(const string& mensaje) {
    int valor;
    while(true) {
        cout << mensaje;
        if(cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        } else {
            cout << "Error: Entrada invalida. Por favor ingrese un numero entero." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string leerCadena(const string& mensaje) {
    string valor;
    cout << mensaje;
    getline(cin, valor);
    return valor;
}

// ========== FUNCIONES DE PERSISTENCIA ==========

bool guardarDatos() {
    ofstream archivo("procesos.dat");
    
    if(!archivo.is_open()) {
        cout << "Error: No se pudo crear el archivo de datos." << endl;
        return false;
    }
    
    Proceso* actual = listaProcesos.getCabeza();
    while(actual != nullptr) {
        archivo << actual->id << "," << actual->nombre << "," 
                << actual->prioridad << "," << actual->memoria << endl;
        actual = actual->siguiente;
    }
    
    archivo.close();
    if(archivo.fail()) {
        cout << "Error: No se pudo guardar correctamente el archivo." << endl;
        return false;
    }
    
    cout << "Datos guardados correctamente en 'procesos.dat'" << endl;
    return true;
}

bool cargarDatos() {
    ifstream archivo("procesos.dat");
    
    if(!archivo.is_open()) {
        cout << "No se encontro archivo de datos previo." << endl;
        return false;
    }
    
    string linea;
    int maxId = 0;
    
    while(getline(archivo, linea)) {
        if(linea.empty()) continue;
        
        size_t pos1 = linea.find(',');
        size_t pos2 = linea.find(',', pos1 + 1);
        size_t pos3 = linea.find(',', pos2 + 1);
        
        if(pos1 == string::npos || pos2 == string::npos || pos3 == string::npos) {
            cout << "Error: Formato incorrecto en archivo de datos." << endl;
            continue;
        }
        
        string idStr = linea.substr(0, pos1);
        string nombre = linea.substr(pos1 + 1, pos2 - pos1 - 1);
        string prioridadStr = linea.substr(pos2 + 1, pos3 - pos2 - 1);
        string memoriaStr = linea.substr(pos3 + 1);
        
        try {
            int id = stoi(idStr);
            int prioridad = stoi(prioridadStr);
            int memoria = stoi(memoriaStr);
            
            listaProcesos.insertarProceso(nombre, prioridad, memoria);
            
            if(id > maxId) {
                maxId = id;
            }
            
        } catch(const exception& e) {
            cout << "Error: Datos corruptos en el archivo." << endl;
        }
    }
    
    archivo.close();
    
    if(maxId > 0) {
        listaProcesos.setUltimoId(maxId);
    }
    
    cout << "Datos cargados correctamente." << endl;
    return true;
}

// ========== INTERFAZ DE USUARIO ==========

void mostrarMenuPrincipal() {
    cout << "\n===================================" << endl;
    cout << "    SISTEMA DE GESTION DE PROCESOS" << endl;
    cout << "===================================" << endl;
    cout << "1. Gestor de Procesos (Lista Enlazada)" << endl;
    cout << "2. Planificador de CPU (Cola de Prioridad)" << endl;
    cout << "3. Gestor de Memoria (Pila)" << endl;
    cout << "4. Guardar Datos" << endl;
    cout << "5. Cargar Datos" << endl;
    cout << "6. Salir" << endl;
    cout << "===================================" << endl;
    cout << "Seleccione una opcion: ";
}

void menuGestorProcesos() {
    int opcion;
    do {
        cout << "\n===================================" << endl;
        cout << "          GESTOR DE PROCESOS" << endl;
        cout << "===================================" << endl;
        cout << "1. Insertar nuevo proceso" << endl;
        cout << "2. Eliminar proceso por ID" << endl;
        cout << "3. Buscar proceso por ID" << endl;
        cout << "4. Modificar prioridad de proceso" << endl;
        cout << "5. Mostrar todos los procesos" << endl;
        cout << "6. Volver al menu principal" << endl;
        cout << "===================================" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero("");
        
        switch(opcion) {
            case 1: {
                string nombre = leerCadena("Ingrese nombre del proceso: ");
                int prioridad = leerEntero("Ingrese prioridad (1-10, 1=mas alta): ");
                int memoria = leerEntero("Memoria requerida (MB): ");
                listaProcesos.insertarProceso(nombre, prioridad, memoria);
                break;
            }
            case 2: {
                cout << "\nLista de procesos disponibles para eliminar:" << endl;
                listaProcesos.mostrarProcesos();
                int id = leerEntero("\nID del proceso a eliminar: ");
                listaProcesos.eliminarProceso(id);
                break;
            }
            case 3: {
                int id = leerEntero("ID del proceso a buscar: ");
                Proceso* proceso = listaProcesos.buscarProceso(id);
                if(proceso != nullptr) {
                    cout << "\nProceso encontrado:" << endl;
                    cout << "ID: " << proceso->id << endl;
                    cout << "Nombre: " << proceso->nombre << endl;
                    cout << "Prioridad: " << proceso->prioridad << endl;
                    cout << "Memoria: " << proceso->memoria << " MB" << endl;
                } else {
                    cout << "Proceso con ID " << id << " no encontrado." << endl;
                }
                break;
            }
            case 4: {
                cout << "\nLista de procesos disponibles para modificar:" << endl;
                listaProcesos.mostrarProcesos();
                int id = leerEntero("\nID del proceso a modificar: ");
                int nuevaPrioridad = leerEntero("Nueva prioridad: ");
                listaProcesos.modificarPrioridad(id, nuevaPrioridad);
                break;
            }
            case 5:
                listaProcesos.mostrarProcesos();
                break;
            case 6:
                cout << "Volviendo al menu principal..." << endl;
                return;
            default:
                cout << "Opcion invalida. Intente nuevamente." << endl;
        }
        cout << endl;
    } while(opcion != 6);
}

void menuPlanificadorCPU() {
    int opcion;
    do {
        cout << "\n===================================" << endl;
        cout << "        PLANIFICADOR DE CPU" << endl;
        cout << "===================================" << endl;
        cout << "1. Agregar proceso a cola de ejecucion" << endl;
        cout << "2. Ejecutar siguiente proceso" << endl;
        cout << "3. Mostrar cola de ejecucion" << endl;
        cout << "4. Volver al menu principal" << endl;
        cout << "===================================" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero("");
        
        switch(opcion) {
            case 1: {
                cout << "\nLista de procesos disponibles para encolar:" << endl;
                listaProcesos.mostrarProcesos();
                int id = leerEntero("\nID del proceso a encolar: ");
                Proceso* proceso = listaProcesos.buscarProceso(id);
                if(proceso != nullptr) {
                    colaCPU.encolarProceso(*proceso);
                    cout << "Proceso '" << proceso->nombre << "' agregado a la cola." << endl;
                } else {
                    cout << "Proceso con ID " << id << " no encontrado." << endl;
                }
                break;
            }
            case 2:
                colaCPU.desencolarProceso();
                break;
            case 3:
                colaCPU.mostrarColaEjecucion();
                break;
            case 4:
                cout << "Volviendo al menu principal..." << endl;
                return;
            default:
                cout << "Opcion invalida. Intente nuevamente." << endl;
        }
        cout << endl;
    } while(opcion != 4);
}

void menuGestorMemoria() {
    int opcion;
    do {
        cout << "\n===================================" << endl;
        cout << "          GESTOR DE MEMORIA" << endl;
        cout << "===================================" << endl;
        cout << "1. Asignar memoria a proceso" << endl;
        cout << "2. Liberar ultimo bloque de memoria" << endl;
        cout << "3. Mostrar estado de memoria" << endl;
        cout << "4. Volver al menu principal" << endl;
        cout << "===================================" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero("");
        
        switch(opcion) {
            case 1: {
                cout << "\nLista de procesos disponibles para asignar memoria:" << endl;
                listaProcesos.mostrarProcesos();
                int id = leerEntero("\nID del proceso: ");
                int memoria = leerEntero("Memoria a asignar (MB): ");
                pilaMemoria.asignarMemoria(id, memoria);
                break;
            }
            case 2:
                pilaMemoria.liberarBloqueMemoria();
                break;
            case 3:
                pilaMemoria.mostrarEstadoMemoria();
                break;
            case 4:
                cout << "Volviendo al menu principal..." << endl;
                return;
            default:
                cout << "Opcion invalida. Intente nuevamente." << endl;
        }
        cout << endl;
    } while(opcion != 4);
}

// ========== FUNCION PRINCIPAL ==========

int main() {
    cout << "Cargando datos previos..." << endl;
    cargarDatos();
    
    int opcion;
    do {
        mostrarMenuPrincipal();
        opcion = leerEntero("");
        
        switch(opcion) {
            case 1:
                menuGestorProcesos();
                break;
            case 2:
                menuPlanificadorCPU();
                break;
            case 3:
                menuGestorMemoria();
                break;
            case 4:
                guardarDatos();
                break;
            case 5:
                cargarDatos();
                break;
            case 6:
                cout << "Saliendo del sistema. ¡Hasta luego!" << endl;
                break;
            default:
                cout << "Opcion invalida. Intente nuevamente." << endl;
        }
        cout << endl;
    } while(opcion != 6);
    
    return 0;
}
