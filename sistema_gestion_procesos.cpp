/*
 * Sistema de Gesti√≥n de Procesos
 *
 * Este programa implementa un peque√±o sistema de gesti√≥n de procesos que
 * utiliza exclusivamente estructuras de datos din√°micas lineales para
 * administrar procesos, planificar su ejecuci√≥n en una CPU y gestionar
 * bloques de memoria.  El dise√±o se inspira en la consigna de un
 * proyecto universitario en el que se deben emplear listas enlazadas,
 * pilas y colas implementadas desde cero.  La aplicaci√≥n ofrece una
 * interfaz por consola y no implementa persistencia de datos; todo el
 * estado se mantiene en memoria mientras el programa se ejecuta.
 *
 * Funcionalidades principales:
 *  - Gestor de procesos: Inserci√≥n, eliminaci√≥n, b√∫squeda y
 *    modificaci√≥n de prioridad.  Internamente se usa una lista
 *    enlazada para almacenar todos los procesos definidos.
 *  - Planificador de CPU: Cola de prioridad (implementada como
 *    lista enlazada ordenada) que permite encolar procesos seg√∫n su
 *    prioridad y desencolarlos en orden para su ‚Äúejecuci√≥n‚Äù.  Tambi√©n
 *    ofrece visualizaci√≥n del estado actual de la cola.
 *  - Gestor de memoria: Pila que simula la asignaci√≥n y liberaci√≥n
 *    de bloques de memoria.  Cada bloque tiene un identificador y
 *    un tama√±o asociado.
 *
 * Para simplificar la interacci√≥n, el usuario puede acceder a un men√∫
 * principal desde el cual se llaman a las diferentes operaciones.
 */

#include <iostream>
#include <string>
#include <limits>

// ===============================
// Definiciones del Gestor de Procesos
// ===============================

struct Process {
    int id;
    std::string name;
    int priority;
};

struct ProcessNode {
    Process data;
    ProcessNode* next;
    ProcessNode(const Process& p) : data(p), next(nullptr) {}
};

class ProcessList {
public:
    ProcessList() : head(nullptr) {}
    ~ProcessList() { clear(); }

    // Inserta un nuevo proceso al final de la lista
    void insert(const Process& p) {
        ProcessNode* newNode = new ProcessNode(p);
        if (!head) {
            head = newNode;
        } else {
            ProcessNode* temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        std::cout << "Proceso insertado con …xito.\n";
    }

    // Elimina un proceso por ID.  Devuelve true si se elimin√≥.
    bool removeById(int id) {
        if (!head) return false;
        ProcessNode* temp = head;
        ProcessNode* prev = nullptr;
        while (temp) {
            if (temp->data.id == id) {
                if (prev) {
                    prev->next = temp->next;
                } else {
                    head = temp->next;
                }
                delete temp;
                return true;
            }
            prev = temp;
            temp = temp->next;
        }
        return false;
    }

    // Busca un proceso por ID o nombre.  Devuelve puntero al proceso encontrado o nullptr.
    ProcessNode* search(int id, const std::string& name) {
        ProcessNode* temp = head;
        while (temp) {
            if (temp->data.id == id || (!name.empty() && temp->data.name == name)) {
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }

    // Modifica la prioridad de un proceso existente
    bool modifyPriority(int id, int newPriority) {
        ProcessNode* node = search(id, "");
        if (node) {
            node->data.priority = newPriority;
            return true;
        }
        return false;
    }

    // Muestra todos los procesos en la lista
    void display() const {
        if (!head) {
            std::cout << "No hay procesos registrados.\n";
            return;
        }
        std::cout << "Lista de procesos registrados:\n";
        std::cout << "ID\tNombre\tPrioridad\n";
        ProcessNode* temp = head;
        while (temp) {
            std::cout << temp->data.id << "\t" << temp->data.name << "\t" << temp->data.priority << "\n";
            temp = temp->next;
        }
    }

    // Elimina todos los nodos de la lista
    void clear() {
        ProcessNode* temp = head;
        while (temp) {
            ProcessNode* nextNode = temp->next;
            delete temp;
            temp = nextNode;
        }
        head = nullptr;
    }

private:
    ProcessNode* head;
};

// ===============================
// Definiciones del Planificador de CPU (Cola de prioridad)
// ===============================

struct QueueNode {
    Process data;
    QueueNode* next;
    QueueNode(const Process& p) : data(p), next(nullptr) {}
};

class CPUQueue {
public:
    CPUQueue() : front(nullptr) {}
    ~CPUQueue() { clear(); }

    // Encola un proceso en la posici√≥n correcta seg√∫n su prioridad
    void enqueue(const Process& p) {
        QueueNode* newNode = new QueueNode(p);
        // Si la cola est√° vac√≠a o la prioridad del nuevo nodo es mayor (menor valor num√©rico) que el front
        if (!front || p.priority < front->data.priority) {
            newNode->next = front;
            front = newNode;
        } else {
            QueueNode* temp = front;
            while (temp->next && temp->next->data.priority <= p.priority) {
                temp = temp->next;
            }
            newNode->next = temp->next;
            temp->next = newNode;
        }
        std::cout << "Proceso encolado con …xito.\n";
    }

    // Desencola el proceso de mayor prioridad (menor valor num√©rico)
    bool dequeue(Process& outProcess) {
        if (!front) return false;
        QueueNode* temp = front;
        outProcess = temp->data;
        front = front->next;
        delete temp;
        return true;
    }

    // Muestra el contenido de la cola
    void display() const {
        if (!front) {
            std::cout << "\nLa cola de la CPU est√° vacÌa.\n";
            return;
        }
        std::cout << "Cola de la CPU (en orden de prioridad):\n";
        std::cout << "ID\tNombre\tPrioridad\n";
        QueueNode* temp = front;
        while (temp) {
            std::cout << temp->data.id << "\t" << temp->data.name << "\t" << temp->data.priority << "\n";
            temp = temp->next;
        }
    }

    // Limpia la cola por completo
    void clear() {
        while (front) {
            QueueNode* temp = front;
            front = front->next;
            delete temp;
        }
    }

private:
    QueueNode* front;
};

// ===============================
// Definiciones del Gestor de Memoria (Pila)
// ===============================

struct MemoryBlock {
    int id;
    size_t size;
};

struct StackNode {
    MemoryBlock block;
    StackNode* next;
    StackNode(const MemoryBlock& b) : block(b), next(nullptr) {}
};

class MemoryStack {
public:
    MemoryStack() : top(nullptr) {}
    ~MemoryStack() { clear(); }

    // Asigna un bloque de memoria (push en la pila)
    void allocate(const MemoryBlock& b) {
        StackNode* newNode = new StackNode(b);
        newNode->next = top;
        top = newNode;
        std::cout << "Bloque de memoria asignado (ID: " << b.id << ").\n";
    }

    // Libera el √∫ltimo bloque asignado (pop de la pila)
    bool release(MemoryBlock& outBlock) {
        if (!top) return false;
        StackNode* temp = top;
        outBlock = temp->block;
        top = top->next;
        delete temp;
        return true;
    }

    // Muestra el estado de la memoria (contenido de la pila)
    void display() const {
        if (!top) {
            std::cout << "No hay bloques de memoria asignados.\n";
            return;
        }
        std::cout << "Estado actual de la memoria (pila):\n";
        std::cout << "ID\tTamaÒo\n";
        StackNode* temp = top;
        while (temp) {
            std::cout << temp->block.id << "\t" << temp->block.size << "\n";
            temp = temp->next;
        }
    }

    // Elimina todos los bloques
    void clear() {
        while (top) {
            StackNode* temp = top;
            top = top->next;
            delete temp;
        }
    }

private:
    StackNode* top;
};

// ===============================
// Funciones de apoyo para leer entradas seguras
// ===============================

// Limpia cualquier entrada err√≥nea y reinicia el flujo de entrada
void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Solicita un entero con validaci√≥n
int promptForInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInput();
            return value;
        } else {
            std::cout << "\nEntrada inv·lida. Intente nuevamente.\n";
            clearInput();
        }
    }
}

// Solicita una cadena no vac√≠a
std::string promptForString(const std::string& prompt) {
    std::string value;
    do {
        std::cout << prompt;
        std::getline(std::cin, value);
        if (value.empty()) {
            std::cout << "\nLa cadena no puede estar vacÌa. Intente nuevamente.\n";
        }
    } while (value.empty());
    return value;
}

// ===============================
// Men√∫s y l√≥gicas de interacci√≥n
// ===============================

void menuProcesos(ProcessList& plist) {
    while (true) {
        std::cout << "\n--- Gestor de Procesos ---\n";
        std::cout << "1. Insertar proceso\n";
        std::cout << "2. Eliminar proceso\n";
        std::cout << "3. Buscar proceso\n";
        std::cout << "4. Modificar prioridad\n";
        std::cout << "5. Mostrar procesos\n";
        std::cout << "0. Volver al men˙ principal\n";
        int choice = promptForInt("Seleccione una opciÛn: ");
        switch (choice) {
        case 1: {
            Process p;
            p.id = promptForInt("Ingrese ID del proceso: ");
            p.name = promptForString("Ingrese nombre del proceso: ");
            p.priority = promptForInt("Ingrese prioridad del proceso (n√∫mero menor = mayor prioridad): ");
            plist.insert(p);
            break;
        }
        case 2: {
            int id = promptForInt("Ingrese el ID del proceso a eliminar: ");
            bool success = plist.removeById(id);
            if (success) {
                std::cout << "\nProceso eliminado exitosamente.\n";
            } else {
                std::cout << "\nProceso no encontrado.\n";
            }
            break;
        }
        case 3: {
            std::cout << "B˙squeda de proceso: puede ingresar ID o nombre (deje el otro campo vacÌo).\n";
            int id = promptForInt("Ingrese ID (0 si no desea usar): ");
            std::string name;
            std::cout << "Ingrese nombre (enter para omitir): ";
            std::getline(std::cin, name);
            // Si id es 0, ignoramos id en b√∫squeda
            ProcessNode* result = plist.search(id == 0 ? -1 : id, name);
            if (result) {
                std::cout << "Proceso encontrado: ID=" << result->data.id
                          << ", Nombre=" << result->data.name
                          << ", Prioridad=" << result->data.priority << "\n";
            } else {
                std::cout << "Proceso no encontrado.\n";
            }
            break;
        }
        case 4: {
            int id = promptForInt("Ingrese ID del proceso cuya prioridad desea modificar: ");
            int newPrio = promptForInt("Ingrese nueva prioridad: ");
            bool mod = plist.modifyPriority(id, newPrio);
            if (mod) {
                std::cout << "Prioridad actualizada.\n";
            } else {
                std::cout << "Proceso no encontrado.\n";
            }
            break;
        }
        case 5:
            plist.display();
            break;
        case 0:
            return;
        default:
            std::cout << "OpciÛn inv·lida, intente nuevamente.\n";
        }
    }
}

void menuCPU(CPUQueue& cpuq, const ProcessList& plist) {
    while (true) {
        std::cout << "\n--- Planificador de CPU ---\n";
        std::cout << "1. Encolar proceso\n";
        std::cout << "2. Desencolar proceso\n";
        std::cout << "3. Mostrar cola\n";
        std::cout << "0. Volver al men˙ principal\n";
        int choice = promptForInt("Seleccione una opciÛn: ");
        switch (choice) {
        case 1: {
            // Para encolar un proceso existente, solicitamos ID
            int id = promptForInt("Ingrese ID del proceso a encolar: ");
            // Intentamos encontrar el proceso en la lista de procesos
            ProcessNode* node = const_cast<ProcessList&>(plist).search(id, "");
            if (!node) {
                std::cout << "El ID ingresado no corresponde a un proceso registrado.\n";
            } else {
                cpuq.enqueue(node->data);
            }
            break;
        }
        case 2: {
            Process out;
            bool success = cpuq.dequeue(out);
            if (success) {
                std::cout << "Proceso desencolado y listo para ejecuciÛn : ID ="
                          << out.id << ", Nombre =" << out.name << ", Prioridad="
                          << out.priority << "\n";
            } else {
                std::cout << "La cola est· vacÌa.\n";
            }
            break;
        }
        case 3:
            cpuq.display();
            break;
        case 0:
            return;
        default:
            std::cout << "OpciÛn inv·lida, intente nuevamente.\n";
        }
    }
}

void menuMemoria(MemoryStack& memStack) {
    while (true) {
        std::cout << "\n--- Gestor de Memoria ---\n";
        std::cout << "1. Asignar bloque\n";
        std::cout << "2. Liberar bloque\n";
        std::cout << "3. Mostrar estado de memoria\n";
        std::cout << "0. Volver al men˙ principal\n";
        int choice = promptForInt("Seleccione una opciÛn: ");
        switch (choice) {
        case 1: {
            MemoryBlock b;
            b.id = promptForInt("Ingrese ID del bloque de memoria: ");
            b.size = (size_t)promptForInt("Ingrese tamaÒo del bloque (en bytes): ");
            memStack.allocate(b);
            break;
        }
        case 2: {
            MemoryBlock released;
            bool success = memStack.release(released);
            if (success) {
                std::cout << "Bloque liberado: ID=" << released.id
                          << ", TamaÒo =" << released.size << "\n";
            } else {
                std::cout << "No hay bloques para liberar.\n";
            }
            break;
        }
        case 3:
            memStack.display();
            break;
        case 0:
            return;
        default:
            std::cout << "\nOpciÛn inv·lida, intente nuevamente.\n";
        }
    }
}

int main() {
	setlocale(LC_ALL, "Spanish");
    ProcessList plist;
    CPUQueue cpuq;
    MemoryStack memStack;

    std::cout << "===== Bienvenido al Sistema de GestiÛn de Procesos =====\n";
    while (true) {
        std::cout << "\n=== Men˙ Principal ===\n";
        std::cout << "1. Gestor de Procesos\n";
        std::cout << "2. Planificador de CPU\n";
        std::cout << "3. Gestor de Memoria\n";
        std::cout << "4. Mostrar todos los procesos\n";
        std::cout << "0. Salir\n";
        int option = promptForInt("Seleccione una opciÛn: ");
        switch (option) {
        case 1:
            menuProcesos(plist);
            break;
        case 2:
            menuCPU(cpuq, plist);
            break;
        case 3:
            menuMemoria(memStack);
            break;
        case 4:
            plist.display();
            break;
        case 0:
            std::cout << "\nSaliendo del sistema. Hasta luego!\n";
            return 0;
        default:
            std::cout << "\nOpciÛn inv·lida, intente nuevamente.\n";
        }
    }
    return 0;
}
