#include <iostream>
#include <string>

using namespace std;



// ==========================================================
// 1. ESTRUCTURAS DE EQUIPO (PILAS Y COLAS)
// ==========================================================
struct NodoDato {
    string info;
    NodoDato* sig;
};

struct PilaEscudos {
    NodoDato* tope = nullptr;
    void push(string vida) { tope = new NodoDato{vida, tope}; }
    void mostrar() {
        if (!tope) { cout << "Vacia"; return; }
        cout << "[Tope: " << tope->info << "]";
    }
};

struct ColaBalas {
    NodoDato *frente = nullptr, *final = nullptr;
    void enqueue(string vida) {
        NodoDato* nuevo = new NodoDato{vida, nullptr};
        if (!frente) frente = final = nuevo;
        else { final->sig = nuevo; final = nuevo; }
    }
    void mostrar() {
        if (!frente) { cout << "Vacia"; return; }
        cout << "[Frente: " << frente->info << "]";
    }
};

// ==========================================================
// 1. ESTRUCTURAS DE EQUIPOS (BANDOS) - INDEPENDIENTES
// ==========================================================
struct EquipoNeon {
    int ip_bando = 1;
    string nombre = "Resistencia Neon";
};

struct EquipoOmega {
    int ip_bando = 2;
    string nombre = "Corporacion OMEGA";
};

// ==========================================================
// 2. ESTRUCTURA DE LISTA PARA ITEMS (ITEMS PARA LA SEGUNDA ENTREGA) Y ARMAS 
// ==========================================================
struct NodoItem {
    string nombre_item;
    NodoItem* siguiente;
};
struct Arma {string nombre; int dano; };

// ==========================================================
// 3. ESTRUCTURAS DE PERSONAJES (SOLOS)
// ==========================================================

struct Juggernaut {
    int ip;
    int vida = 150;     // Vida base especial
    int escudo = 100;
    Arma arma = {"Canon de Riel", 15};
    string clase = "Juggernaut";
};

struct Espectro {
    int ip;
    int vida = 60;      // Vida base especial
    int escudo = 100;   // Escudo solicitado
    Arma arma = {"Daga de datos", 10};
    string clase = "Espectro"; // Fantasma
};

struct Hacker {
    int ip;
    int vida = 100;     // Vida base especial
    int escudo = 100;   // Escudo solicitado
    Arma arma = {"Virus de Enlace", 30};
    string clase = "Hacker";
};

struct Healer {
    int ip;
    int vida = 60;
    int escudo = 100;
    Arma arma = {"Varita de Sauco", 5};
    string clase = "Sacerdote";
};

struct Ejecutor {
    int ip;
    int vida = 80;
    int escudo = 100;
    Arma arma = {"Cuchillas sangrientas", 40};
    string clase = "Asesino";
};

// ==========================================================
// 4. NODO DEL SISTEMA (CRUD)
// ==========================================================
struct NodoSistema {
    int ip_personaje;
    int ip_bando;
    int vida;
    int escudo;
    int dano;
    string clase;
    string nombre_bando;
    string n_arma;
    
    NodoSistema* siguiente = nullptr;
};

// ==========================================================
// 5. FUNCIONES
// ==========================================================

// --- CREATE (Crear e Inyectar) ---
void crear(NodoSistema* &cabeza) {
    int tipo, bando_sel, ip_p;
    cout << "\n--- REGISTRO DE OPERATIVO ---" << endl;
    cout << "Bando (1: Neon | 2: OMEGA): "; cin >> bando_sel;
    cout << "Clase (1: Juggernaut | 2: Espectro | 3: Hacker | 4: Healer | 5: Asesino): "; cin >> tipo;
    cout << "IP Unica: "; cin >> ip_p;

    NodoSistema* nuevo = new NodoSistema;
    nuevo->ip_personaje = ip_p;

    // Datos del Bando
    if (bando_sel == 1) {
        EquipoNeon b;
        nuevo->ip_bando = b.ip_bando;
        nuevo->nombre_bando = b.nombre;
    } else {
        EquipoOmega b;
        nuevo->ip_bando = b.ip_bando;
        nuevo->nombre_bando = b.nombre;
    }

    // Datos del Personaje
    if (tipo == 1) {
        Juggernaut p;
        nuevo->vida = p.vida;
        nuevo->escudo = p.escudo;
        nuevo->clase = p.clase;
        nuevo->n_arma= p.arma.nombre;
    } else if (tipo == 2) {
        Espectro p;
        nuevo->vida = p.vida;
        nuevo->escudo = p.escudo;
        nuevo->clase = p.clase;
        nuevo->n_arma= p.arma.nombre;
    } else if (tipo == 3){
        Hacker p;
        nuevo->vida = p.vida;
        nuevo->escudo = p.escudo;
        nuevo->clase = p.clase;
        nuevo->n_arma= p.arma.nombre;
    } else if (tipo == 4){
        Healer p;
        nuevo->vida = p.vida;
        nuevo->escudo = p.escudo;
        nuevo->clase = p.clase;
        nuevo->n_arma= p.arma.nombre;
    } else {
        Ejecutor p;
        nuevo->vida = p.vida;
        nuevo->escudo = p.escudo;
        nuevo->clase = p.clase;
        nuevo->n_arma= p.arma.nombre;
    }

    nuevo->siguiente = cabeza;
    cabeza = nuevo;
    cout << "-> Operativo de inyectado con exito.\n";
}

// --- READ (Leer / Ver) ---
void leer(NodoSistema* cabeza) {
    if (!cabeza) { cout << "\nNo hay operativos en red.\n"; return; }
    NodoSistema* aux = cabeza;
    while (aux != nullptr) {
        cout << "[IP: " << aux->ip_personaje << " | Bando: " << aux->nombre_bando 
             << " | " << aux->clase << " | HP: " << aux->vida << " | SHIELD: " << aux->escudo
              << " | Arma: " << aux->n_arma << "]" << endl;
        aux = aux->siguiente;
    }
}

// --- UPDATE (Actualizar Vida y Escudo) ---
void actualizar(NodoSistema* cabeza, int ip_buscar) {
    NodoSistema* aux = cabeza;
    while (aux != nullptr) {
        if (aux->ip_personaje == ip_buscar) {
            cout << "\nOperativo encontrado. Modificando IP: " << ip_buscar << endl;
            cout << "Ingrese nueva Vida (HP): "; cin >> aux->vida;
            cout << "Ingrese nuevo Escudo (SHIELD): "; cin >> aux->escudo;
            cout << "-> Datos actualizados correctamente.\n";
            return;
        }
        aux = aux->siguiente;
    }
    cout << "-> IP no localizada.\n";
}

// --- DELETE (Borrar) ---
void borrar(NodoSistema* &cabeza, int ip_buscar) {
    NodoSistema* aux = cabeza;
    NodoSistema* anterior = nullptr;

    while (aux != nullptr && aux->ip_personaje != ip_buscar) {
        anterior = aux;
        aux = aux->siguiente;
    }

    if (!aux) { cout << "No se encontro la IP para eliminar.\n"; return; }

    if (!anterior) cabeza = cabeza->siguiente;
    else anterior->siguiente = aux->siguiente;

    delete aux;
    cout << "-> Operativo eliminado de la memoria.\n";
}



// ==========================================================
// 6. MENU
// ==========================================================
int main() {
    NodoSistema* yggdrasil = nullptr;
    int opcion, id_busqueda;

    do {
        cout << "\n--- NUCLEO YGGDRAZIL ---" << endl;
        cout << "1. Crear Operativo" << endl;
        cout << "2. Ver Operativos" << endl;
        cout << "3. Actualizar Vida/Escudo" << endl;
        cout << "4. Eliminar Operativo" << endl;
        cout << "5. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: crear(yggdrasil); break;
            case 2: leer(yggdrasil); break;
            case 3: 
                cout << "Ingrese IP a buscar para actualizar: "; cin >> id_busqueda;
                actualizar(yggdrasil, id_busqueda); break;
            case 4: 
                cout << "Ingrese IP a borrar: "; cin >> id_busqueda;
                borrar(yggdrasil, id_busqueda); break;
        }
    } while (opcion != 5);

    return 0;
}