#include <ios>
#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;
/*==================================
    Variebles de respeto en el juego
====================================*/
const int MAX_VIDA = 400;
const int MAX_ESCUDO = 200;
const int MAX_MIEMBROS_BANDO = 60;

int contadorNeon = 0;
int contadorOmega = 0;
int turnoActual = 1;
int contadorTurno = 1;
int dadoTurno = 1;
int accionesRestantes = 0;
int ultimoTurnoHeroeNeon = -10; 
int ultimoTurnoHeroeOmega = -10;

/*======================================
    Estructuras utilizadas y constantes
========================================*/
struct EquipoNeon {
    int ip_bando = 1;
    string nombre = "Resistencia Neon";
};

struct EquipoOmega {
    int ip_bando = 2;
    string nombre = "Corporacion OMEGA";
};

struct Arma {
    int id_arma;
    string nombre;
    int dano;
    int bullets_actuales;
    int cargador_maximo;
};

struct NodoArma {
    Arma info_arma;
    NodoArma* siguiente = nullptr;
};

struct NodoItem {
    string nombre_item;
    string tipo_efecto;
    int valor_efecto;
    NodoItem* siguiente = nullptr;
};

struct Especie {
    int id;
    string nombre;
    int salud;
    int fuerza;
    int defensa;
};

struct Heroe {
    int id;
    string nombre;
    int fortaleza;
    int salud;
};

struct Perfil {
    int id;
    string nombre;
    int fortaleza;
    int salud;
};

struct ControlRaiz {
    int bandoDominanteActual = 0; 
    int rondasConsecutivas = 0;   
};

const Especie Especies[] = {
    {1, "Humano", 120, 70, 60},
    {2, "Elfo", 80, 65, 90},
    {3, "Enano", 150, 100, 40},
    {4, "Mago", 95, 55, 50},
    {5, "Caballero", 130, 90, 60},
    {6, "Ladron", 70, 60, 110},
    {7, "Orco", 90, 80, 50},
    {8, "Troll", 60, 40, 80},
    {9, "Vampiro", 75, 25, 60},
    {10, "Golem", 110, 200, 20},
    {11, "Licántropo", 85, 30, 70},
    {12, "Necrofago", 200, 300, 50}
};

const Heroe heroes[] = {
    {1, "Comandante Cipher", 250, 200},
    {2, "Fantasma \"Null Pointer\"", 120, 180},
    {3, "Titán de Acero", 400, 400},
    {4, "Oráculo de Datos", 180, 140}
};


struct Personaje {
    string clase;
    int vida;
    int vida_max;
    int escudo;
    string habilidad;
    Arma* arma_assigned = nullptr;
    NodoItem* inventario = nullptr;
    NodoArma* cola_ataques = nullptr; 
    NodoItem* pila_escudos = nullptr;
};

Personaje obtenerPlantillaPersonaje(int tipo) {
    Personaje p;
    if (tipo == 1) {
        p.clase = "Juggernaut"; p.vida = 150; p.vida_max = 150; p.escudo = 100; p.habilidad = "Fuerza Bruta";
    } else if (tipo == 2) {
        p.clase = "Espectro"; p.vida = 60; p.vida_max = 60; p.escudo = 100; p.habilidad = "Sigilo Optico";
    } else if (tipo == 3) {
        p.clase = "Hacker"; p.vida = 100; p.vida_max = 100; p.escudo = 100; p.habilidad = "Control de Mentes";
    } else if (tipo == 4) {
        p.clase = "Sacerdote"; p.vida = 60; p.vida_max = 60; p.escudo = 100; p.habilidad = "Curacion Amiga";
    } else if (tipo == 5) {
        p.clase = "Asesino"; p.vida = 100; p.vida_max = 100; p.escudo = 100; p.habilidad = "Golpe Subito";
    } else {
        p.clase = "Personalizado"; p.vida = 100; p.vida_max = 100; p.escudo = 100; p.habilidad = "Modificacion Libre";
    }
    return p;
}

struct RegistroOperativo {
    int ip_personaje;
    int ip_bando;
    string nombre_bando;
    Personaje operativo;
};

/*==============================================
                Arbol y funciones
================================================ */
class ArbolB;
class NodoB;
void mostrarInventario(NodoItem* cabeza);
struct RegistroOperativo;
void borrar(ArbolB& arbol, int ip_buscar, NodoItem*& itemsCaidos);


Arma popColaAtaques(NodoArma*& cola);
void pushPilaEscudos(NodoItem*& pila, const string& nombre, const string& tipo, int valor);
void ejecutarAtaque(Personaje& atacante, Personaje& objetivo, int& ip_bando_objetivo);
void simularCombateEnNodo(NodoB* nodo, ArbolB& arbol, NodoItem*& itemsCaidos);
void procesarDano(Personaje& p, int dano);
void asignarArmaYColas(Personaje& p, Arma* arma);



class NodoB {
    RegistroOperativo llaves[3];
    NodoB* hijos[4];
    int n;
    bool hoja;
    int t;
public:
    NodoB(int _t, bool _hoja) : t(_t), hoja(_hoja), n(0) {
        for (int i = 0; i < 4; ++i) hijos[i] = nullptr;
    }

    int cantidad() const { return n; }
    bool esHoja() const { return hoja; }
    RegistroOperativo& llave(int i) { return llaves[i]; }
    NodoB*& hijo(int i) { return hijos[i]; }

    bool contieneClave(int k, int& idx) {
        idx = 0;
        while (idx < n && llaves[idx].ip_personaje < k) ++idx;
        return (idx < n && llaves[idx].ip_personaje == k);
    }

    void insertarNoLleno(const RegistroOperativo& k) {
        int i = n - 1;
        if (hoja) {
            while (i >= 0 && llaves[i].ip_personaje > k.ip_personaje) {
                llaves[i + 1] = llaves[i];
                --i;
            }
            llaves[i + 1] = k;
            ++n;
        } else {
            while (i >= 0 && llaves[i].ip_personaje > k.ip_personaje) --i;
            ++i;
            if (hijos[i] != nullptr && hijos[i]->cantidad() == 2 * t - 1) {
                dividirHijo(i, hijos[i]);
                if (llaves[i].ip_personaje < k.ip_personaje) ++i;
            }
            hijos[i]->insertarNoLleno(k);
        }
    }

    void dividirHijo(int idx, NodoB* y) {
        NodoB* z = new NodoB(y->t, y->hoja);
        z->n = t - 1;
        for (int j = 0; j < t - 1; ++j) z->llaves[j] = y->llaves[j + t];
        if (!y->hoja) {
            for (int j = 0; j < t; ++j) z->hijos[j] = y->hijos[j + t];
        }
        y->n = t - 1;
        for (int j = n; j >= idx + 1; --j) hijos[j + 1] = hijos[j];
        hijos[idx + 1] = z;
        for (int j = n - 1; j >= idx; --j) llaves[j + 1] = llaves[j];
        llaves[idx] = y->llaves[t - 1];
        ++n;
    }

    bool eliminarRec(int k) {
        int idx;
        if (contieneClave(k, idx)) {
            if (hoja) {
                for (int i = idx; i < n - 1; ++i) llaves[i] = llaves[i + 1];
                --n;
                return true;
            }
            NodoB* hijoIzq = hijos[idx];
            NodoB* hijoDer = hijos[idx + 1];
            if (hijoIzq != nullptr && hijoIzq->cantidad() >= t) {
                RegistroOperativo pred = hijoIzq->llaves[hijoIzq->n - 1];
                llaves[idx] = pred;
                return hijoIzq->eliminarRec(pred.ip_personaje);
            }
            if (hijoDer != nullptr && hijoDer->cantidad() >= t) {
                RegistroOperativo succ = hijoDer->llaves[0];
                llaves[idx] = succ;
                return hijoDer->eliminarRec(succ.ip_personaje);
            }
            if (hijoIzq != nullptr && hijoDer != nullptr) {
                hijoIzq->llaves[hijoIzq->n] = llaves[idx];
                ++hijoIzq->n;
                for (int i = 0; i < hijoDer->n; ++i) hijoIzq->llaves[hijoIzq->n + i] = hijoDer->llaves[i];
                hijoIzq->n += hijoDer->n;
                if (!hijoIzq->hoja) {
                    for (int i = 0; i <= hijoDer->n; ++i) hijoIzq->hijos[hijoIzq->n - hijoDer->n + i] = hijoDer->hijos[i];
                }
                for (int i = idx; i < n - 1; ++i) llaves[i] = llaves[i + 1];
                --n;
                delete hijoDer;
                return hijoIzq->eliminarRec(k);
            }
        }

        int childIdx = 0;
        while (childIdx < n && k > llaves[childIdx].ip_personaje) ++childIdx;
        NodoB* child = hijos[childIdx];
        if (child == nullptr) return false;
        return child->eliminarRec(k);
    }

    void recorrerRec() {
        for (int i = 0; i < n; ++i) {
            if (!hoja) hijos[i]->recorrerRec();
            RegistroOperativo& op = llaves[i];
            cout << "[IP: " << op.ip_personaje << " | " << op.nombre_bando
                 << " | " << op.operativo.clase << " | HP: " << op.operativo.vida
                 << " | ESCUDO: " << op.operativo.escudo;
            if (op.operativo.arma_assigned != nullptr) {
                cout << " | Arma: " << op.operativo.arma_assigned->nombre
                     << " (" << op.operativo.arma_assigned->bullets_actuales << "/"
                     << op.operativo.arma_assigned->cargador_maximo << ")";
            } else {
                cout << " | Arma: Ninguna";
            }
            cout << " | Hab: " << op.operativo.habilidad << " | Items: ";
            mostrarInventario(op.operativo.inventario);
            cout << "]\n";
        }
        if (!hoja) hijos[n]->recorrerRec();
    }

    void limpiarRec() {
        for (int i = 0; i <= n; ++i) {
            if (hijos[i] != nullptr) {
                hijos[i]->limpiarRec();
                delete hijos[i];
                hijos[i] = nullptr;
            }
        }
        for (int i = 0; i < n; ++i) {
            NodoItem* item = llaves[i].operativo.inventario;
            while (item != nullptr) {
                NodoItem* sig = item->siguiente;
                delete item;
                item = sig;
            }
            llaves[i].operativo.inventario = nullptr;
        }
        n = 0;
    }

    void mostrarNodoJerarquico(int nivel) {
        string indent(nivel * 2, ' ');
        cout << indent << "[NODO - Nivel " << nivel << " | " << n << " operativo(s)]\n";
        
        for (int i = 0; i < n; ++i) {
            RegistroOperativo& op = llaves[i];
            cout << indent << "  +-- [IP: " << op.ip_personaje
                 << " | " << op.nombre_bando
                 << " | " << op.operativo.clase
                 << " | HP: " << op.operativo.vida
                 << " | ESCUDO: " << op.operativo.escudo;
            if (op.operativo.arma_assigned != nullptr) {
                cout << " | Arma: " << op.operativo.arma_assigned->nombre
                     << " (" << op.operativo.arma_assigned->bullets_actuales << "/"
                     << op.operativo.arma_assigned->cargador_maximo << ")";
            } else {
                cout << " | Arma: Ninguna";
            }
            cout << " | Hab: " << op.operativo.habilidad << " | Items: ";
            mostrarInventario(op.operativo.inventario);
            cout << "]\n";
        }
        
        if (!hoja) {
            for (int i = 0; i <= n; ++i) {
                if (hijos[i] != nullptr) {
                    cout << indent << "  |\n";
                    hijos[i]->mostrarNodoJerarquico(nivel + 1);
                }
            }
        }
        cout << indent << "---[FIN NODO]---\n";
    }

    friend class ArbolB;
};

class ArbolB {
    NodoB* raiz;
    int t;
public:
    ArbolB(int _t) : raiz(nullptr), t(_t) {}

    void insertar(const RegistroOperativo& k) {
        if (raiz == nullptr) {
            raiz = new NodoB(t, true);
            raiz->llave(0) = k;
            raiz->n = 1;
            return;
        }
        if (raiz->cantidad() == 2 * t - 1) {
            NodoB* s = new NodoB(t, false);
            s->hijo(0) = raiz;
            s->dividirHijo(0, raiz);
            int i = 0;
            if (s->llave(0).ip_personaje < k.ip_personaje) ++i;
            s->hijo(i)->insertarNoLleno(k);
            raiz = s;
        } else {
            raiz->insertarNoLleno(k);
        }
    }

    RegistroOperativo* buscar(int k) {
        if (raiz == nullptr) return nullptr;
        NodoB* actual = raiz;
        while (actual != nullptr) {
            int idx = 0;
            while (idx < actual->cantidad() && actual->llave(idx).ip_personaje < k) ++idx;
            if (idx < actual->cantidad() && actual->llave(idx).ip_personaje == k) {
                return &actual->llave(idx);
            }
            if (actual->esHoja()) break;
            actual = actual->hijo(idx);
        }
        return nullptr;
    }

    bool eliminar(int k) {
        if (raiz == nullptr) return false;
        bool ok = raiz->eliminarRec(k);
        if (raiz->cantidad() == 0 && !raiz->esHoja()) {
            NodoB* old = raiz;
            raiz = raiz->hijo(0);
            delete old;
        }
        return ok;
    }

    void recorrer() {
        if (raiz != nullptr) raiz->recorrerRec();
    }

    void limpiar() {
        if (raiz != nullptr) {
            raiz->limpiarRec();
            delete raiz;
            raiz = nullptr;
        }
    }

    void mostrarJerarquico() {
        if (raiz != nullptr) {
            cout << "\n=== ARBOL YGGDRASIL (ESTRUCTURA JERARQUICA) ===\n";
            cout << "Neon: " << contadorNeon << " | Omega: " << contadorOmega << "\n";
            raiz->mostrarNodoJerarquico(0);
        } else {
            cout << "[VACIO] No hay operativos en la red.\n";
        }
    }

    NodoB* buscarNodoConteniendo(int k) {
        if (raiz == nullptr) return nullptr;
        NodoB* actual = raiz;
        while (actual != nullptr) {
            int idx = 0;
            while (idx < actual->cantidad() && actual->llave(idx).ip_personaje < k) ++idx;
            if (idx < actual->cantidad() && actual->llave(idx).ip_personaje == k) {
                return actual;
            }
            if (actual->esHoja()) break;
            actual = actual->hijo(idx);
        }
        return nullptr;
    }

    void simularCombate(NodoItem*& itemsCaidos) {
        if (raiz != nullptr) {
            simularCombateEnNodo(raiz, *this, itemsCaidos);
        } else {
            cout << "[AVISO] El árbol está vacío. No hay combate que simular." << endl;
        }
    }
    NodoB* getRaiz() const { return raiz; }
};

bool esNombreValido(const string& texto) {
    if (texto.empty()) return false;
    for (char c : texto) {
        if (isdigit(c)) return false;
    }
    return true;
}

bool esBandoPermitidoPorTurno(int bando) {
    return (turnoActual == 1 && bando == 1) || (turnoActual == 2 && bando == 2);
}

string nombreDelTurnoActual() {
    return (turnoActual == 1) ? "Resistencia Neon" : "Corporacion OMEGA";
}

void avanzarTurno() {
    turnoActual = (turnoActual == 1) ? 2 : 1;
    contadorTurno++;
    accionesRestantes = 1 + rand() % 3;
    cout << "\n=======================================================\n";
    cout << "[!] SISTEMA: Cambio de red detectado.\n";
    cout << "[!] Turno #" << contadorTurno << " transferido a: " << nombreDelTurnoActual() << "\n";
    cout << "[!] DADO LANZADO: El bando tiene " << accionesRestantes << " acciones para este turno.\n";
    cout << "=======================================================\n";
}

int lanzarDadoTurno() {
    dadoTurno = 1 + rand() % 3;
    return dadoTurno;
}

string descripcionDado(int valor) {
    if (valor == 1) return "Accion de creacion/inyeccion";
    if (valor == 2) return "Accion de ataque/disparo";
    return "Accion de apoyo/habilidad/recarga";
}

int leerEnteroSeguro() {
    int numero;
    while (true) {
        cin >> numero;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[Error] Entrada invalida. Ingrese solo valores numericos: ";
        } else {
            return numero;
        }
    }
}

string leerTextoSeguro(const string& mensaje) {
    string entrada;
    while (true) {
        cout << mensaje;
        getline(cin, entrada);
        if (esNombreValido(entrada)) {
            return entrada;
        }
        cout << "[Error] El nombre no puede contener numeros ni quedar vacio. Intente de nuevo.\n";
    }
}

bool existeIDArma(Arma arsenalBase[], int tamBase, NodoArma* listaArmasCustom, int id_buscar) {
    for (int i = 0; i < tamBase; ++i) {
        if (arsenalBase[i].id_arma == id_buscar) return true;
    }
    NodoArma* aux = listaArmasCustom;
    while (aux != nullptr) {
        if (aux->info_arma.id_arma == id_buscar) return true;
        aux = aux->siguiente;
    }
    return false;
}

Arma* buscarArmaPorID(Arma arsenalBase[], int tamBase, NodoArma* listaArmasCustom, int id_buscar) {
    for (int i = 0; i < tamBase; ++i) {
        if (arsenalBase[i].id_arma == id_buscar) return &arsenalBase[i];
    }
    NodoArma* aux = listaArmasCustom;
    while (aux != nullptr) {
        if (aux->info_arma.id_arma == id_buscar) return &(aux->info_arma);
        aux = aux->siguiente;
    }
    return nullptr;
}

void mostrarArsenalCompleto(Arma arsenalBase[], int tamBase, NodoArma* listaArmasCustom) {
    cout << "\n--- ARSENAL COMPLETO EN RED ---" << endl;
    for (int i = 0; i < tamBase; ++i) {
        cout << "[Base] ID: " << arsenalBase[i].id_arma << " | " << arsenalBase[i].nombre
             << " | Dano: " << arsenalBase[i].dano
             << " | Municion: " << arsenalBase[i].bullets_actuales << "/" << arsenalBase[i].cargador_maximo << endl;
    }
    NodoArma* aux = listaArmasCustom;
    while (aux != nullptr) {
        cout << "[Custom] ID: " << aux->info_arma.id_arma << " | " << aux->info_arma.nombre
             << " | Dano: " << aux->info_arma.dano
             << " | Municion: " << aux->info_arma.bullets_actuales << "/" << aux->info_arma.cargador_maximo << endl;
        aux = aux->siguiente;
    }
}

void agregarItem(NodoItem*& inventario, string nombre, string tipoEfecto, int valor) {
    NodoItem* nuevo = new NodoItem;
    nuevo->nombre_item = nombre;
    nuevo->tipo_efecto = tipoEfecto;
    nuevo->valor_efecto = valor;
    nuevo->siguiente = inventario;
    inventario = nuevo;
}

void mostrarInventario(NodoItem* cabeza) {
    if (!cabeza) { cout << "Vacio"; return; }
    NodoItem* aux = cabeza;
    while (aux != nullptr) {
        cout << "[" << aux->nombre_item << " (" << aux->tipo_efecto << ": " << aux->valor_efecto << ")] ";
        aux = aux->siguiente;
    }
}

bool aplicarPlantillaInstruccion(Personaje& operativo, int categoria, int opcion) {
    if (categoria == 1) {
        if (opcion < 1 || opcion > 12) return false;
        const Especie& especie = Especies[opcion - 1];
        operativo.clase = especie.nombre;
        operativo.vida = especie.salud;
        operativo.vida_max = especie.salud;
        operativo.escudo = especie.defensa;
        operativo.habilidad = "Plantilla de Especie";
        return true;
    }
    if (categoria == 2) {
        if (opcion < 1 || opcion > 4) return false;
        const Heroe& heroe = heroes[opcion - 1];
        operativo.clase = heroe.nombre;
        operativo.vida = heroe.salud;
        operativo.vida_max = heroe.salud;
        operativo.escudo = heroe.fortaleza / 2;
        operativo.habilidad = "Habilidad de Heroe";
        return true;
    }
    return false;
}

RegistroOperativo* buscarOperativoPorIP(ArbolB& arbol, int ip_buscar) {
    return arbol.buscar(ip_buscar);
}

void cargarLideresPredeterminados(ArbolB& arbol, Arma arsenalBase[]) {
    RegistroOperativo liderNeon;
    liderNeon.ip_personaje = 10;
    liderNeon.ip_bando = 1;
    liderNeon.nombre_bando = "Resistencia Neon";
    liderNeon.operativo.clase = "Juggernaut Lider";
    liderNeon.operativo.vida = 300;
    liderNeon.operativo.vida_max = 300;
    liderNeon.operativo.escudo = 200;
    liderNeon.operativo.habilidad = "Fuerza Bruta";
    asignarArmaYColas(liderNeon.operativo, &arsenalBase[0]);
    liderNeon.operativo.inventario = nullptr;
    agregarItem(liderNeon.operativo.inventario, "Inyector de Nanobots", "Vida", 100);
    arbol.insertar(liderNeon);
    contadorNeon = 1;

    RegistroOperativo liderOmega;
    liderOmega.ip_personaje = 20;
    liderOmega.ip_bando = 2;
    liderOmega.nombre_bando = "Corporacion OMEGA";
    liderOmega.operativo.clase = "Asesino Director";
    liderOmega.operativo.vida = 350;
    liderOmega.operativo.vida_max = 350;
    liderOmega.operativo.escudo = 200;
    liderOmega.operativo.habilidad = "Golpe Subito";
    asignarArmaYColas(liderOmega.operativo, &arsenalBase[4]);
    liderOmega.operativo.inventario = nullptr;
    agregarItem(liderOmega.operativo.inventario, "Tarjeta de Acceso Alfa", "Escudo", 150);
    arbol.insertar(liderOmega);
    contadorOmega = 1;
}

bool ejecutarDisparo(ArbolB& arbol, NodoItem*& itemsCaidos) {
    cout << "\n--- PROTOCOLO DE ATAQUE EN RED ---\n";
    cout << "Ingrese IP del Operativo Atacante: ";
    int ipAtacante = leerEnteroSeguro();

    RegistroOperativo* atacante = buscarOperativoPorIP(arbol, ipAtacante);
    if (!atacante) {
        cout << "[ERROR] El atacante no existe en el sistema.\n";
        return false;
    }
    if (atacante->ip_bando != turnoActual) {
        cout << "[ERROR] ¡Acceso Denegado! Este operativo pertenece al bando enemigo.\n";
        return false;
    }
    if (atacante->operativo.arma_assigned == nullptr) {
        cout << "[AVISO] El operativo IP " << ipAtacante << " no tiene un arma asignada. ¡No puede disparar!\n";
        return false;
    }

    Arma* arma = atacante->operativo.arma_assigned;
    if (arma->bullets_actuales <= 0) {
        cout << "[¡CLICK!] El arma '" << arma->nombre << "' no tiene balas. Requiere RECUPERAR MUNICION (Opcion 7).\n";
        return false;
    }

    cout << "Ingrese IP del Operativo Objetivo (Enemigo): ";
    int ipObjetivo = leerEnteroSeguro();
    if (ipAtacante == ipObjetivo) {
        cout << "[Aviso] No puedes atacarte a ti mismo.\n";
        return false;
    }

    RegistroOperativo* objetivo = buscarOperativoPorIP(arbol, ipObjetivo);
    if (!objetivo) {
        cout << "[ERROR] El objetivo no fue localizado en la red.\n";
        return false;
    }
    if (atacante->ip_bando == objetivo->ip_bando) {
        cout << "[Aviso] No puedes atacar a tus aliados.\n";
        return false;
    }
    if (objetivo->operativo.vida <= 0) {
        cout << "[AVISO CHIP] El objetivo IP " << ipObjetivo << " ya se encuentra desmantelado (0 HP).\n";
        return false;
    }

    // Verificar que atacante y objetivo estén en el mismo nodo
    NodoB* nodoAtacante = arbol.buscarNodoConteniendo(ipAtacante);
    NodoB* nodoObjetivo = arbol.buscarNodoConteniendo(ipObjetivo);
    if (nodoAtacante != nodoObjetivo) {
        cout << "[ERROR] El atacante y el objetivo no se encuentran en el mismo nodo de la red.\n";
        cout << "-> Solo puedes disparar a enemigos dentro de tu mismo nodo.\n";
        return false;
    }

    arma->bullets_actuales--;
    cout << "\n[¡PUM!] " << atacante->operativo.clase << " (IP: " << ipAtacante << ") dispara '" << arma->nombre << "' contra IP: " << ipObjetivo << ".\n";
    cout << "Balas restantes en el cargador: " << arma->bullets_actuales << "/" << arma->cargador_maximo << "\n";

    int danoProvocado = arma->dano;
    cout << "-> Impacto inminente de " << danoProvocado << " unidades de dano digital.\n";

    if (objetivo->operativo.escudo > 0) {
        if (objetivo->operativo.escudo >= danoProvocado) {
            objetivo->operativo.escudo -= danoProvocado;
            danoProvocado = 0;
            cout << "-> El escudo de plasma absorbio todo el impacto.\n";
        } else {
            danoProvocado -= objetivo->operativo.escudo;
            cout << "-> El escudo colapso. Absorbio " << objetivo->operativo.escudo << " de dano.\n";
            objetivo->operativo.escudo = 0;
        }
    }
    if (danoProvocado > 0) {
        objetivo->operativo.vida -= danoProvocado;
        if (objetivo->operativo.vida < 0) objetivo->operativo.vida = 0;
        cout << "-> La integridad del objetivo sufrio danos directos.\n";
    }

    cout << ">> Estatus del Objetivo (IP: " << ipObjetivo << ") -> HP: " << objetivo->operativo.vida << " | ESCUDO: " << objetivo->operativo.escudo << "\n";
    if (objetivo->operativo.vida <= 0) {
        cout << "\n[¡ELIMINACIoN CRÍTICA!] El operativo IP " << ipObjetivo << " ha caído en batalla.\n";
        cout << "-> Soltando armamento e inventario al suelo...\n";
        borrar(arbol, ipObjetivo, itemsCaidos);
    }
    return true;
}

bool ejecutarRecarga(ArbolB& arbol) {
    cout << "\n--- RECARGA DE ENERGIA/MUNICION ---\n";
    cout << "Ingrese IP del Operativo a recargar: ";
    int ipOp = leerEnteroSeguro();

    RegistroOperativo* op = buscarOperativoPorIP(arbol, ipOp);
    if (!op) {
        cout << "[Error] El operativo no existe.\n";
        return false;
    }
    if (op->ip_bando != turnoActual) {
        cout << "[ERROR] Este operativo pertenece al bando enemigo.\n";
        return false;
    }
    if (op->operativo.arma_assigned == nullptr) {
        cout << "[Aviso] Este operativo no cuenta con armas en su ranura.\n";
        return false;
    }

    Arma* arma = op->operativo.arma_assigned;
    if (arma->bullets_actuales == arma->cargador_maximo) {
        cout << "[Aviso] El cargador de '" << arma->nombre << "' ya está lleno ("
             << arma->bullets_actuales << "/" << arma->cargador_maximo << "). No es necesario recargar.\n";
        return false;
    }

    arma->bullets_actuales = arma->cargador_maximo;
    cout << "-> Cargador reabastecido. '" << arma->nombre << "' lista con " << arma->bullets_actuales << "/" << arma->cargador_maximo << " balas.\n";
    return true;
}

bool ejecutarHabilidad(ArbolB& arbol, NodoItem*& itemsCaidos) {
    cout << "\n--- ACTIVACION DE HABILIDAD ESPECIAL ---\n";
    cout << "Ingrese IP del Operativo que usara su habilidad: ";
    int ipOp = leerEnteroSeguro();

    RegistroOperativo* ejecutor = buscarOperativoPorIP(arbol, ipOp);
    if (!ejecutor) {
        cout << "[Error] El operativo no existe en la red.\n";
        return false;
    }
    if (ejecutor->ip_bando != turnoActual) {
        cout << "[ERROR] Este operativo pertenece al bando enemigo.\n";
        return false;
    }
    if (ejecutor->operativo.vida <= 0) {
        cout << "[Error] Un operativo desmantelado no puede usar habilidades.\n";
        return false;
    }

    string clase = ejecutor->operativo.clase;
    string hab = ejecutor->operativo.habilidad;
    cout << "-> Operativo de Clase '" << clase << "' activando: [" << hab << "]\n";

    if (hab == "Fuerza Bruta" || clase == "Juggernaut") {
        ejecutor->operativo.escudo += 50;
        if (ejecutor->operativo.escudo > MAX_ESCUDO) ejecutor->operativo.escudo = MAX_ESCUDO;
        cout << "[!] Escudo de plasma sobrecargado. Nuevo Escudo: " << ejecutor->operativo.escudo << "/" << MAX_ESCUDO << "\n";
    } else if (hab == "Sigilo Optico" || clase == "Espectro") {
        if (ejecutor->operativo.arma_assigned != nullptr) {
            ejecutor->operativo.arma_assigned->bullets_actuales = ejecutor->operativo.arma_assigned->cargador_maximo;
            cout << "[!] Hackeo de cargador exitoso. '" << ejecutor->operativo.arma_assigned->nombre << "' rellenada al instante.\n";
        } else {
            cout << "[Aviso] El espectro no tiene un arma equipada para rellenar municion.\n";
        }
    } else if (hab == "Control de Mentes" || clase == "Hacker") {
        cout << "Ingrese IP del Objetivo Enemigo para robar datos (balas): ";
        int ipObj = leerEnteroSeguro();
        RegistroOperativo* objetivo = buscarOperativoPorIP(arbol, ipObj);
        if (!objetivo || objetivo->ip_bando == ejecutor->ip_bando || ipObj == ipOp) {
            cout << "[Aviso] Objetivo invalido o es un aliado.\n";
            return false;
        }
        if (ejecutor->operativo.arma_assigned == nullptr || objetivo->operativo.arma_assigned == nullptr) {
            cout << "[Aviso] Uno de los dos operativos involucrados no posee armas.\n";
            return false;
        }
        int balasRobadas = min(5, objetivo->operativo.arma_assigned->bullets_actuales);
        objetivo->operativo.arma_assigned->bullets_actuales -= balasRobadas;
        ejecutor->operativo.arma_assigned->bullets_actuales += balasRobadas;
        if (ejecutor->operativo.arma_assigned->bullets_actuales > ejecutor->operativo.arma_assigned->cargador_maximo) {
            ejecutor->operativo.arma_assigned->bullets_actuales = ejecutor->operativo.arma_assigned->cargador_maximo;
        }
        cout << "[!] Exito. Has hackeado el arma del rival, extrayendo " << balasRobadas << " balas directamente a tu cargador.\n";
    } else if (hab == "Curacion Amiga" || clase == "Sacerdote") {
        cout << "Ingrese IP del Operativo Aliado a quien deseas curar: ";
        int ipAliado = leerEnteroSeguro();
        RegistroOperativo* aliado = buscarOperativoPorIP(arbol, ipAliado);
        if (!aliado || aliado->ip_bando != ejecutor->ip_bando || ipAliado == ipOp) {
            cout << "[Aviso] No puedes curarte a ti mismo ni a un rival. Selecciona un aliado real.\n";
            return false;
        }
        if (aliado->operativo.vida <= 0) {
            cout << "[Aviso] No puedes revivir operativos completamente desmantelados (0 HP).\n";
            return false;
        }
        aliado->operativo.vida += 50;
        if (aliado->operativo.vida > aliado->operativo.vida_max) {
            aliado->operativo.vida = aliado->operativo.vida_max;
        }
        cout << "[!] Inyeccion de nanobots completada. Aliado IP " << ipAliado << " recuperado a " << aliado->operativo.vida << " HP.\n";
    } else if (hab == "Golpe Subito" || clase == "Asesino") {
        cout << "Ingrese IP del Objetivo Enemigo a ejecutar: ";
        int ipObj = leerEnteroSeguro();
        RegistroOperativo* objetivo = buscarOperativoPorIP(arbol, ipObj);
        if (!objetivo || objetivo->ip_bando == ejecutor->ip_bando || ipObj == ipOp) {
            cout << "[Aviso] No puedes atacar a tus aliados.\n";
            return false;
        }
        if (objetivo->operativo.vida <= 0) {
            cout << "[Aviso] El objetivo ya esta destruido.\n";
            return false;
        }
        objetivo->operativo.vida -= 40;
        if (objetivo->operativo.vida < 0) objetivo->operativo.vida = 0;
        cout << "[!] ¡FILTRACIoN CRÍTICA! Golpe sorpresa inflige 40 de dano evadiendo sus defensas de escudo.\n";
        cout << ">> Estatus del Rival (IP: " << ipObj << ") -> HP: " << objetivo->operativo.vida << " | ESCUDO: " << objetivo->operativo.escudo << "\n";
        if (objetivo->operativo.vida <= 0) {
            cout << "\n[¡ELIMINACIoN CRÍTICA!] El golpe subito destruyo al operativo IP " << ipObj << ".\n";
            borrar(arbol, ipObj, itemsCaidos);
        }
    } else {
        cout << "[Aviso] Esta clase no tiene un modulo de accion programado o es de Modificacion Libre.\n";
    }
    return true;
}

bool crear(ArbolB& arbol, Arma arsenalBase[], int tamBase, NodoArma*& listaArmasCustom, NodoItem*& itemsCaidos) {
    int bando_sel = turnoActual;
    if ((bando_sel == 1 && contadorNeon >= MAX_MIEMBROS_BANDO) || (bando_sel == 2 && contadorOmega >= MAX_MIEMBROS_BANDO)) {
        cout << "[ERROR CRÍTICO] Servidor lleno para el bando activo.\n";
        return false;
    }

    cout << "\n--- REGISTRO DE OPERATIVO ---\n";
    cout << "Turno activo: " << nombreDelTurnoActual() << "\n";
    cout << "IP Unica: ";
    int ip_p = leerEnteroSeguro();
    if (ip_p == 10 || ip_p == 20) {
        cout << "[Error] IPs 10 y 20 reservadas.\n";
        return false;
    }
    if (buscarOperativoPorIP(arbol, ip_p) != nullptr) {
        cout << "[Error] La IP ya existe en la red.\n";
        return false;
    }

    RegistroOperativo nuevoOp;
    nuevoOp.ip_personaje = ip_p;
    nuevoOp.ip_bando = bando_sel;
    nuevoOp.nombre_bando = (bando_sel == 1) ? "Resistencia Neon" : "Corporacion OMEGA";
    nuevoOp.operativo.inventario = nullptr;

    
    cout << "\n=== Seleccione Base de crear su personaje ===" << endl;
    cout << "1. Especie" << endl;
    cout << "2. Heroe(solo se podra cada 3 turnos 1 solo)" << endl;
    cout << "Opcion: ";
    int cat = leerEnteroSeguro();

    string nombreBase= "";
    int hpBase, defBase;

    if(cat==1){
        cout << "---Lista de Especies---" << endl;
        for(int i=0; i<12; ++i){
            cout <<  Especies[i].id << " " << Especies[i].nombre << " (HP: " << Especies[i].salud << " |Res: " << Especies[i].defensa << ")\n";
        }
        cout << "Seleccione su especie: " << endl;

        int op;

        while (true) {
            cout << "Seleccione su especie (1-12): ";
            op = leerEnteroSeguro();
            if (op >= 1 && op <= 12) {
                break; // Opción válida
            }
            cout << "[Error] Opcion no valida. Intente de nuevo.\n";
        }
        const Especie& esp = Especies[op - 1];
        nombreBase = esp.nombre;
        hpBase = esp.salud;
        defBase = esp.defensa;
    }
    else if(cat == 2){
        int& ultimoTurnoHeroe =(bando_sel ==1) ? ultimoTurnoHeroeNeon : ultimoTurnoHeroeOmega;
        if(contadorTurno - ultimoTurnoHeroe < 3) {
            cout << "[ERROR] en las ultimas rondas ingresaste un Heroe." << endl;
            cout << "Rondas restantes para ingresar a un Heroe: " << 3 - (contadorTurno - ultimoTurnoHeroe) << endl;
            return false;
        }
        cout << "\n---Lista de Heroes---" << endl;
        for (int i = 0; i<4;i++){
            cout << heroes[i].id << ". " << heroes[i].nombre << "(HP: " << heroes[i].salud << " |Res: " << heroes[i].fortaleza << ")\n";
        }
        
        int op;

        while (true) {
            cout << "Seleccione su Heroe (1-4): ";
            op = leerEnteroSeguro();
            if (op >= 1 && op <= 4) {
                break;
            }
            cout << "[Error] Opcion no valida. Intente de nuevo.\n";
        }
        
        const Heroe& heroe = heroes[op - 1];
        nombreBase=heroe.nombre;
        hpBase = heroe.salud;
        defBase = heroe.fortaleza;

        ultimoTurnoHeroe = contadorTurno;
    }
    else {
        cout << "Opcion no valida" << endl;
        return false;
    }

    cout << "\n--- SELECCIONE EL ROL PARA ESTE OPERATIVO (" << nombreBase << ") ---" << endl;
    cout << "1. Juggernaut (Tanque / Fuerza Bruta)\n";
    cout << "2. Espectro (Infiltrador / Sigilo Optico)\n";
    cout << "3. Hacker (Control / Hackeo de Mentes)\n";
    cout << "4. Sacerdote (Soporte / Curacion Amiga)\n";
    cout << "5. Asesino (Dano Critico / Golpe Subito)\n";
    cout << "6. Rol Personalizado (Habilidad configurable)\n";
    int tipoRol;

    while (true) {
        cout << "Seleccione Rol (1-6): ";
        tipoRol = leerEnteroSeguro();
        if (tipoRol >= 1 && tipoRol <= 6) {
            break; // Opción válida
        }
        cout << "[Error] Rol invalido. Intente de nuevo.\n";
    }

    Personaje rPLantilla = obtenerPlantillaPersonaje(tipoRol);

    int vidaTotal = hpBase + rPLantilla.vida;
    int defTotal = defBase + rPLantilla.escudo;
    nuevoOp.operativo.clase = nombreBase + " ("  + rPLantilla.clase + ")";
    nuevoOp.operativo.vida = vidaTotal;
    nuevoOp.operativo.vida_max = vidaTotal;
    nuevoOp.operativo.escudo = defTotal;
    nuevoOp.operativo.habilidad = rPLantilla.habilidad;

    if(tipoRol == 6){
        cin.ignore(numeric_limits<streamsize>:: max(), '\n');
        string nombrePersonalizado = leerTextoSeguro("Ingresa el nombre de tu personaje: ");
        string habilidadPersonalizada = leerTextoSeguro("Ingresa el nombre de su habilidad especial: ");

        nuevoOp.operativo.clase = nombrePersonalizado;
        nuevoOp.operativo.habilidad = habilidadPersonalizada;
    }
    else {
        nuevoOp.operativo.clase = nombreBase + " (" + rPLantilla.clase + ")";
        nuevoOp.operativo.habilidad = rPLantilla.habilidad;
    }
    
    nuevoOp.operativo.vida = vidaTotal;
    nuevoOp.operativo.vida_max = vidaTotal;
    nuevoOp.operativo.escudo = defTotal;

    cout << "\nArsenal disponible:" << endl;
    mostrarArsenalCompleto(arsenalBase, tamBase, listaArmasCustom);

    Arma* armaSel = nullptr;
    while (true) {
        cout << "Ingrese ID del Arma a equipar (0 para ninguna): ";
        int id_arma_sel = leerEnteroSeguro();
        if (id_arma_sel == 0) break;

        armaSel = buscarArmaPorID(arsenalBase, tamBase, listaArmasCustom, id_arma_sel);
        if (armaSel != nullptr) {
            break; 
        }
        cout << "[Aviso] Arma no encontrada en el arsenal. Intente de nuevo.\n";
    }

    if (bando_sel == 1) ++contadorNeon; else ++contadorOmega;
    arbol.insertar(nuevoOp);
    cout << "Operativo registrado con exito para " << nuevoOp.nombre_bando << ".\n";
    return true;
}

void leer(ArbolB& arbol) {
    arbol.mostrarJerarquico();
}

bool ejecutarUsoItemVida(ArbolB& arbol) {
    cout << "\n--- PROTOCOLO DE REGENERACIoN DE BIOMASA (ITEMS) ---\n";
    cout << "Ingrese IP del Operativo que usara el item: ";
    int ipOp = leerEnteroSeguro();

    RegistroOperativo* op = buscarOperativoPorIP(arbol, ipOp);
    if (!op) {
        cout << "[Error] El operativo no existe en la red.\n";
        return false;
    }
    if (op->ip_bando != turnoActual) {
        cout << "[ERROR] Este operativo pertenece al bando enemigo.\n";
        return false;
    }
    if (op->operativo.vida <= 0) {
        cout << "[Error] Un operativo desmantelado no puede consumir objetos.\n";
        return false;
    }
    if (op->operativo.inventario == nullptr) {
        cout << "[Aviso] El inventario de este operativo esta vacio.\n";
        return false;
    }

    NodoItem* actual = op->operativo.inventario;
    NodoItem* anterior = nullptr;
    while (actual != nullptr) {
        if (actual->tipo_efecto == "Vida") break;
        anterior = actual;
        actual = actual->siguiente;
    }
    if (actual == nullptr) {
        cout << "[Aviso] El operativo no tiene ningun item de tipo 'Vida' en su inventario.\n";
        return false;
    }

    cout << "-> Utilizando item: " << actual->nombre_item << " (Restaura +" << actual->valor_efecto << " HP).\n";
    op->operativo.vida += actual->valor_efecto;
    if (op->operativo.vida > op->operativo.vida_max) {
        op->operativo.vida = op->operativo.vida_max;
        cout << "[!] ¡Curacion al maximo de tu clase!\n";
    }
    cout << ">> Nueva integridad del operativo -> HP: " << op->operativo.vida << "/" << op->operativo.vida_max << "\n";

    if (anterior == nullptr) {
        op->operativo.inventario = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }
    delete actual;
    cout << "-> El objeto se ha gastado y removido del inventario.\n";
    return true;
}

void borrar(ArbolB& arbol, int ip_buscar, NodoItem*& itemsCaidos) {
    if (ip_buscar == 10 || ip_buscar == 20) {
        cout << "-> Error critico: No puedes eliminar al lider del bando.\n";
        return;
    }

    RegistroOperativo* op = buscarOperativoPorIP(arbol, ip_buscar);
    if (!op) {
        cout << "No se encontro la IP para eliminar.\n";
        return;
    }

    if (op->ip_bando == 1) {
        --contadorNeon;
    } else {
        --contadorOmega;
    }

    NodoItem* itemAux = op->operativo.inventario;
    while (itemAux != nullptr) {
        NodoItem* sigItem = itemAux->siguiente;
        itemAux->siguiente = itemsCaidos;
        itemsCaidos = itemAux;
        itemAux = sigItem;
    }
    op->operativo.inventario = nullptr;

    if (op->operativo.arma_assigned != nullptr) {
        cout << "-> El arma '" << op->operativo.arma_assigned->nombre << "' ha sido devuelta al arsenal disponible.\n";
    }
    arbol.eliminar(ip_buscar);
    cout << "-> Cupo liberado. Poblacion actual -> Neon: " << contadorNeon << "/60 | Omega: " << contadorOmega << "/60.\n";
}

void borrar_todo(ArbolB& arbol) {
    arbol.limpiar();
    contadorNeon = 0;
    contadorOmega = 0;
}

void borrar_armas_custom(NodoArma*& inicio) {
    NodoArma* aux;
    while (inicio != nullptr) {
        aux = inicio;
        inicio = inicio->siguiente;
        delete aux;
    }
}

void borrar_items_caidos(NodoItem*& inicio) {
    NodoItem* aux;
    while (inicio != nullptr) {
        aux = inicio;
        inicio = inicio->siguiente;
        delete aux;
    }
}

void procesarDano(Personaje& p, int danoBase) {
    if (danoBase <= 0) return;

    // 1. Cálculo de mitigación por resistencia
    float factorMitigacion = 100.0f / (100.0f + p.escudo); 
    int danoFinal = static_cast<int>(danoBase * factorMitigacion);

    if (danoFinal < 1 && danoBase > 0) danoFinal = 1;

    cout << "-> Resistencia (" << p.escudo << ") mitiga el impacto: Daño reducido de " 
         << danoBase << " a " << danoFinal << ".\n";

    while (danoFinal > 0 && p.pila_escudos != nullptr) {
        NodoItem* escudoActivo = p.pila_escudos;
        cout << "-> Escudo de item '" << escudoActivo->nombre_item << "' absorbiendo daño...\n";
        
        if (escudoActivo->valor_efecto >= danoFinal) {
            escudoActivo->valor_efecto -= danoFinal;
            danoFinal = 0;
            cout << "-> Escudo resistio. Valor restante: " << escudoActivo->valor_efecto << "\n";
        } else {
            danoFinal -= escudoActivo->valor_efecto;
            cout << "-> ¡Escudo destruido! Excedente de daño: " << danoFinal << "\n";
            p.pila_escudos = escudoActivo->siguiente;
            delete escudoActivo;
        }
    }
    if (danoFinal > 0) {
        p.vida -= danoFinal;
        if (p.vida < 0) p.vida = 0;
        cout << "-> Impacto directo a la biomasa: " << p.vida << "/" << p.vida_max << " HP restante.\n";
    }
}

Arma popColaAtaques(NodoArma*& cola) {
    if (cola == nullptr) {
        Arma vacia;
        vacia.id_arma = -1;
        vacia.nombre = "Vacio";
        vacia.dano = 0;
        vacia.bullets_actuales = 0;
        vacia.cargador_maximo = 0;
        return vacia;
    }
    
    NodoArma* frente = cola;
    Arma armaExtraida = frente->info_arma; 
    
    cola = frente->siguiente;
    delete frente; 
    
    return armaExtraida;
}

void pushPilaEscudos(NodoItem*& pila, const string& nombre, const string& tipo, int valor) {
    NodoItem* nuevo = new NodoItem;
    nuevo->nombre_item = nombre;
    nuevo->tipo_efecto = tipo;
    nuevo->valor_efecto = valor;
    nuevo->siguiente = pila;
    pila = nuevo;
}


void asignarArmaYColas(Personaje& p, Arma* arma) {
    if (arma == nullptr) {
        p.arma_assigned = nullptr;
        return;
    }
    
    p.arma_assigned = arma;
    
    NodoArma* aux = p.cola_ataques;
    while (aux != nullptr) {
        NodoArma* sig = aux->siguiente;
        delete aux;
        aux = sig;
    }
    p.cola_ataques = nullptr;
    
    for (int k = 0; k < 5; ++k) {
        NodoArma* nuevo = new NodoArma;
        nuevo->info_arma = *arma; // Copia del arma
        nuevo->siguiente = nullptr;
        
        if (p.cola_ataques == nullptr) {
            p.cola_ataques = nuevo;
        } else {
            NodoArma* ultimo = p.cola_ataques;
            while (ultimo->siguiente != nullptr) {
                ultimo = ultimo->siguiente;
            }
            ultimo->siguiente = nuevo;
        }
    }
    
    cout << "-> Arma '" << arma->nombre << "' asignada. Cola de ataques cargada con 5 disparos." << endl;
}

void ejecutarAtaque(Personaje& atacante, Personaje& objetivo, int& ip_bando_objetivo) {
    Arma armaAtaque = popColaAtaques(atacante.cola_ataques);
    if (armaAtaque.dano <= 0) {
        cout << "[AVISO] El atacante no tiene ataques encolados." << endl;
        return;
    }
    
    cout << "\n[ATAQUE AUTOMATICO] " << atacante.clase
         << " ataca con '" << armaAtaque.nombre << "' (Dano: " << armaAtaque.dano << ")" << endl;
    
    string nombreArma = armaAtaque.nombre;
    transform(nombreArma.begin(), nombreArma.end(), nombreArma.begin(), ::tolower);
    
    if (nombreArma.find("control mental") != string::npos || 
        nombreArma.find("troyano") != string::npos) {
        int bandoOriginal = ip_bando_objetivo;
        ip_bando_objetivo = (ip_bando_objetivo == 1) ? 2 : 1;
        cout << "[!] ¡TROYANO ACTIVADO! El objetivo ha cambiado de bando: "
             << bandoOriginal << " -> " << ip_bando_objetivo << endl;
    } else {
        cout << "-> Aplicando " << armaAtaque.dano << " de dano al objetivo..." << endl;
        procesarDano(objetivo, armaAtaque.dano); 
    }
}

void simularCombateEnNodo(NodoB* nodo, ArbolB& arbol, NodoItem*& itemsCaidos) {
    if (nodo == nullptr) return;
    cout << "--- Analizando Nodo (Cantidad de operativos: " << nodo->cantidad() << ") ---" << endl;    

    for (int i = 0; i < nodo->cantidad(); ++i) {
        RegistroOperativo& atacante = nodo->llave(i);
        
        if (atacante.operativo.vida <= 0) {
            cout << "  [Skip] IP " << atacante.ip_personaje << " está desmantelado." << endl;
            continue;
        }
        
        for (int j = 0; j < nodo->cantidad(); ++j) {
            if (i == j) continue;
            
            RegistroOperativo& objetivo = nodo->llave(j);
            
            
            if (objetivo.operativo.vida <= 0) continue;
            
            if (atacante.ip_bando == objetivo.ip_bando) continue;
            
            
            if (atacante.operativo.cola_ataques == nullptr) {
                cout << "  [Skip] IP " << atacante.ip_personaje << " no tiene ataques encolados." << endl;
                continue;
            };
            
            if (atacante.ip_personaje > objetivo.ip_personaje) {
                cout << "  [COMBATE] IP " << atacante.ip_personaje << " (Bando " << atacante.ip_bando
                     << ") ataca a IP " << objetivo.ip_personaje << " (Bando " << objetivo.ip_bando << ")" << endl;
                
                int& bandoObjetivo = objetivo.ip_bando;
                
                ejecutarAtaque(atacante.operativo, objetivo.operativo, bandoObjetivo);
                
                cout << ">> Estado del objetivo (IP: " << objetivo.ip_personaje
                     << ") -> HP: " << objetivo.operativo.vida
                     << " | ESCUDO: " << objetivo.operativo.escudo << endl;
                
                // Si el HP del objetivo llega a 0, eliminar del árbol
                if (objetivo.operativo.vida <= 0) {
                    cout << "\n[!] ¡ELIMINACION! El operativo IP " << objetivo.ip_personaje
                         << " ha sido destruido en combate." << endl;
                    borrar(arbol, objetivo.ip_personaje, itemsCaidos);
                }
            }
        }
    }
    
    if (!nodo->esHoja()) {
        for (int i = 0; i <= nodo->cantidad(); ++i) {
            if (nodo->hijo(i) != nullptr) {
                simularCombateEnNodo(nodo->hijo(i), arbol, itemsCaidos);
            }
        }
    }
}

void mostrarIntroduccion() {
    cout << "========================================================================\n";
    cout << "                    Epoca 2099 - NEO-KOREA (RED SUBTERRÁNEA)             \n";
    cout << "========================================================================\n\n";
    cout << " El viejo mundo ha colapsado. Las calles destellan bajo luces de neon artificial\n";
    cout << " mientras la megacorporacion OMEGA controla cada aspecto de la vida biologica.\n";
    cout << " Desde las sombras, la Resistencia Neon libra una guerra cibernetica silenciosa,\n";
    cout << " intentando desmantelar los servidores de control mental de la corporacion.\n\n";
    cout << " Ambos bandos se conectan a traves del NUCLEO YGGDRAZIL, una supercomputadora\n";
    cout << " cuantica donde los operativos hackean la reality, forjan armas digitales\n";
    cout << " y modifican sus propios escudos de plasma para sobrevivir.\n\n";
    cout << " [ALERTA DE TRAFICO]: Los cortafuegos del Nucleo solo toleran un maximo de \n";
    cout << " 60 firmas de red activas por faccion simultaneamente antes de colapsar.\n\n";
    cout << " Tu terminal se acaba de enlazar al nucleo. El destino de la red esta en tus manos...\n";
    cout << "========================================================================\n\n";
}

bool verificarVictoriaRaiz(NodoB* raiz, ControlRaiz& estadoRaiz) {
    if (raiz == nullptr || raiz->cantidad() == 0) {
        estadoRaiz.rondasConsecutivas = 0;
        estadoRaiz.bandoDominanteActual = 0;
        return false;
    }

    int primerBando = raiz->llave(0).ip_bando;

    for (int i = 0; i < raiz->cantidad(); i++) {
        if (raiz->llave(i).ip_bando != primerBando) {
            estadoRaiz.rondasConsecutivas = 0;
            estadoRaiz.bandoDominanteActual = 0;
            return false; 
        }
    }
    // Evaluar el acumulado de rondas consecutivas
    if (estadoRaiz.bandoDominanteActual == primerBando) {
        estadoRaiz.rondasConsecutivas++;
    } else {
        estadoRaiz.bandoDominanteActual = primerBando;
        estadoRaiz.rondasConsecutivas = 1;
    }

    cout << "\n[CONTROL DE RAÍZ] El Bando " << primerBando 
         << " (" << (primerBando == 1 ? "Resistencia Neon" : "Corporación OMEGA") << ")"
         << " domina la raíz por " << estadoRaiz.rondasConsecutivas << " ronda(s) consecutiva(s).\n";

    // Victoria a las 3 rondas
    if (estadoRaiz.rondasConsecutivas >= 3) {
        cout << "\n======================================================\n";
        cout << " ¡VICTORIA DE DOMINIO! El Bando " << primerBando 
             << " ha mantenido la raíz por 3 rondas seguidas!\n";
        cout << "======================================================\n";
        return true; 
    }

    return false;
}

bool verificarVictoriaPorPoblacion(int neon, int omega) {

    if (neon + omega >= MAX_MIEMBROS_BANDO) {
        cout << "\n======================================================\n";
        cout << " ¡CAPACIDAD MÁXIMA DE RED ALCANZADA (" << MAX_MIEMBROS_BANDO << " OPERATIVOS)! \n";
        cout << " Evaluando dominancia de tropas en el sistema...\n";
        
        if (neon > omega) {
            cout << " ¡VICTORIA DE LA RESISTENCIA NEÓN! (" << neon << " vs " << omega << ")\n";
        } else if (omega > neon) {
            cout << " ¡VICTORIA DE LA CORPORACIÓN OMEGA! (" << omega << " vs " << neon << ")\n";
        } else {
            cout << " ¡EMPATE TÉCNICO! Ambos bandos cuentan con " << neon << " operativos.\n";
        }
        cout << "======================================================\n";
        return true;
    }
    return false;
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    mostrarIntroduccion();

    int seleccionInicio = 0;
    while (seleccionInicio != 1 && seleccionInicio != 2) {
        cout << " [SISTEMA EN ESPERA DE ACCION]\n";
        cout << " 1. Continuar y Enlazar al Nucleo\n";
        cout << " 2. Desconectarse (Salir)\n";
        cout << " Seleccione una opcion: ";
        seleccionInicio = leerEnteroSeguro();
        if (seleccionInicio == 2) {
            cout << "\n Desconexion segura completada. Saliendo de la matrix...\n";
            return 0;
        } else if (seleccionInicio != 1) {
            cout << "[Error] Codigo de comando invalido. Intente de nuevo.\n\n";
        }
    }

    cout << "\n Connecting... [OK] \n Cargando protocolos de red corporativos y rebeldes...\n";

    ArbolB yggdrasil(2);
    NodoArma* arsenalCustom = nullptr;
    NodoItem* sueloItemsCaidos = nullptr;

    int opcion = -1;
    const int TAM_BASE = 10;

    Arma arsenalBase[TAM_BASE] = {
        {101, "Canon de Riel", 45, 5, 5},
        {102, "Daga de datos", 15, 30, 30},
        {103, "Virus de Enlace", 35, 10, 10},
        {104, "Varita de Sauco", 10, 50, 50},
        {105, "Cuchillas sangrientas", 55, 12, 12},
        {106, "Canon de Plasma Pesado", 180, 50, 50},
        {107, "Rifle Francotirador Gauss", 250, 120, 120},
        {108, "Espada de Iones", 130, 200, 2},
        {109, "Granada de Pulso Electromagnético", 90, 10, 30},
        {110, "Inyector de Malware", 60, 300, 15}
    };

    cargarLideresPredeterminados(yggdrasil, arsenalBase);
    turnoActual = (rand() % 2 == 0) ? 1 : 2;
    accionesRestantes = 1 + rand() % 3;

    cout << "\n[!] Conexion establecida. El primer turno es para: " << nombreDelTurnoActual() << "\n";
    cout << "[!] Dado inicial lanzado: Tienes " << accionesRestantes << " acciones disponibles.\n";

    ControlRaiz estadoRaiz;

    while (opcion != 0) {
        if (accionesRestantes <= 0) {
            avanzarTurno();
            if (verificarVictoriaRaiz(yggdrasil.getRaiz(), estadoRaiz)|| verificarVictoriaPorPoblacion(contadorNeon, contadorOmega)) {
                cout << "\n[SISTEMA] Cerrando conexión por victoria absoluta.\n";
                break;
            }
        }
        cout << "\n--- NUCLEO YGGDRAZIL ---" << endl;
        cout << "Turno #" << contadorTurno << " | Le toca a: " << nombreDelTurnoActual() << endl;
        cout << "Acciones Restantes: " << accionesRestantes << " << " << endl;
        cout << "1. Registrar/Crear Operativo (1pto de accion)" << endl;
        cout << "2. Ver Operativos en Red (Gratis)" << endl;
        cout << "3. Eliminar Sistema Cuantico (Reiniciar servidores)" << endl;
        cout << "4. Ver Arsenal de Armas (Gratis)" << endl;
        cout << "5. Ver Items Tirados en el Suelo(1pto de accion)" << endl;
        cout << "6. [MECANICA] SIMULAR DISPARO (1pto de accion)" << endl;
        cout << "7. [MECANICA] RECARGAR ARMA (1pto de accion)" << endl;
        cout << "8. [MECANICA] USAR HABILIDAD ESPECIAL DE LA CLASE(1pto de accion)" << endl;
        cout << "9. [MECANICA] CONSUMIR OBJETO DE CURACION (1pto de accion)" << endl;
        cout << "10. [FASE 2] SIMULAR COMBATE AUTOMATICO EN NODOS (1pto de accion)" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEnteroSeguro();

        if (opcion == 1) {
            if (crear(yggdrasil, arsenalBase, TAM_BASE, arsenalCustom, sueloItemsCaidos)) {
                accionesRestantes--;
            }
        } else if (opcion == 2) {
            leer(yggdrasil);
        } else if (opcion == 3) {
            cout << "Borrando todo el ordenador cuantico...\n";
            borrar_todo(yggdrasil);
            borrar_items_caidos(sueloItemsCaidos);
            cargarLideresPredeterminados(yggdrasil, arsenalBase);
            cout << "-> Barrido completo. Servidores reiniciados al estado nativo.\n";
            accionesRestantes = 1 + rand() % 3;
        } else if (opcion == 4) {
            mostrarArsenalCompleto(arsenalBase, TAM_BASE, arsenalCustom);
        } else if (opcion == 5) {
            cout << "\n--- ITEMS EN EL SUELO DESATADOS ---\n";
            mostrarInventario(sueloItemsCaidos);
            accionesRestantes--;
            cout << "\n";
        } else if (opcion == 6) {
            if (ejecutarDisparo(yggdrasil, sueloItemsCaidos)) {
                accionesRestantes--;
            }
        } else if (opcion == 7) {
            if (ejecutarRecarga(yggdrasil)) {
                accionesRestantes--;
            }
        } else if (opcion == 8) {
            if (ejecutarHabilidad(yggdrasil, sueloItemsCaidos)) {
                accionesRestantes--;
            }
        } else if (opcion == 9) {
            if (ejecutarUsoItemVida(yggdrasil)) {
                accionesRestantes--;
            }
        } else if (opcion == 10) {
            cout << "\n--- INICIANDO SIMULACION DE COMBATE AUTOMATICO ---" << endl;
            yggdrasil.simularCombate(sueloItemsCaidos); 
            accionesRestantes--;
        } else if (opcion == 0) {
            cout << "Saliendo del sistema Yggdrasil.\n";
        } else {
            cout << "Opcion no valida.\n";
        }
    }

    borrar_todo(yggdrasil);
    borrar_armas_custom(arsenalCustom);
    borrar_items_caidos(sueloItemsCaidos);
    return 0;
}
