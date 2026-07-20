#include <iostream>
#include <string>
#include <limits> 
#include <cctype>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

// Constantes globales para los límites solicitados
const int MAX_VIDA = 400;
const int MAX_ESCUDO = 200;
const int MAX_MIEMBROS_BANDO = 60; 


// Contadores globales para el control de población en red
int contadorNeon = 0;
int contadorOmega = 0;
int turnoActual = 1;
int contadorTurno = 1;
int dadoTurno = 1;
int accionesRestantes = 0;

/*==========================================================
1. ESTRUCTURAS DE EQUIPOS 
==========================================================*/
struct EquipoNeon {
    int ip_bando = 1;
    string nombre = "Resistencia Neon";
};

struct EquipoOmega {
    int ip_bando = 2;
    string nombre = "Corporacion OMEGA";
};

/*==========================================================
2A. ESTRUCTURA DE ARMAS Y ITEMS
============================================================*/
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

/*==========================================================
2B. CATÁLOGO ADICIONAL DE INSTRUCCIONES
============================================================*/
struct EscudoInstruccion {
    int id;
    string nombre;
    string tipo;
    int resistencia;
    int duracion;
    int costo;
};

struct EspecieInstruccion {
    int id;
    string nombre;
    int salud;
    int fuerza;
    int defensa;
    int rapidez;
};

struct HeroeInstruccion {
    int id;
    string nombre;
    int fortaleza;
    int salud;
    int rapidez;
};

struct PerfilInstruccion {
    int id;
    string nombre;
    int fortaleza;
    int salud;
    int rapidez;
};

const EscudoInstruccion escudosInstruccion[] = {
    {1, "Barrera de Energía Cinética", "Escudo_Personal", 150, 200, 10},
    {2, "Proyector de Campo Deflector", "Escudo_Área", 300, 100, 40},
    {3, "Blindaje Reactivo", "Armadura_Física", 120, 300, 5},
    {4, "Cortafuegos Cuántico", "Ciber_Defensa", 250, 500, 0},
    {5, "Módulo de Camuflaje Óptico", "Evasión", 50, 150, 0}
};

const EspecieInstruccion especiesInstruccion[] = {
    {1, "Humano", 120, 70, 60, 60},
    {2, "Elfo", 80, 65, 90, 90},
    {3, "Enano", 150, 100, 40, 40},
    {4, "Mago", 95, 55, 50, 50},
    {5, "Caballero", 130, 90, 60, 60},
    {6, "Ladrón", 70, 60, 110, 110},
    {7, "Orco", 90, 80, 50, 50},
    {8, "Troll", 60, 40, 80, 80},
    {9, "Vampiro", 75, 25, 60, 60},
    {10, "Golem", 110, 200, 20, 20},
    {11, "Licántropo", 85, 30, 70, 70},
    {12, "Necrófago", 200, 300, 50, 50}
};

const HeroeInstruccion heroesInstruccion[] = {
    {1, "Comandante Cipher", 250, 200, 100},
    {2, "Fantasma \"Null Pointer\"", 120, 180, 180},
    {3, "Titán de Acero", 400, 400, 30},
    {4, "Oráculo de Datos", 180, 140, 140}
};

const PerfilInstruccion personajesInstruccion[] = {
    {1, "Hacker de Combate", 80, 110, 110},
    {2, "Infantería Pesada", 150, 40, 40},
    {3, "Ingeniero de Nodos", 100, 70, 70},
    {4, "Dron Centinela", 120, 130, 130},
    {5, "Virus Mutante", 180, 90, 90}
};

/*==========================================================
3. ESTRUCTURA ÚNICA DE PERSONAJE
============================================================*/
struct Personaje {
    string clase;
    int vida;
    int vida_max;
    int escudo;
    string habilidad;
    Arma* arma_assigned = nullptr; 
    NodoItem* inventario = nullptr; 
};

Personaje obtenerPlantillaPersonaje(int tipo) {
    Personaje p;
    
    if (tipo == 1) {
        p.clase = "Juggernaut"; p.vida = 150; p.vida_max = 150; p.escudo = 100; p.habilidad = "Fuerza Bruta";
    } 
    else if (tipo == 2) {
        p.clase = "Espectro"; p.vida = 60; p.vida_max = 60; p.escudo = 100; p.habilidad = "Sigilo Optico";
    } 
    else if (tipo == 3) {
        p.clase = "Hacker"; p.vida = 100; p.vida_max = 100; p.escudo = 100; p.habilidad = "Control de Mentes";
    } 
    else if (tipo == 4) {
        p.clase = "Sacerdote"; p.vida = 60; p.vida_max = 60; p.escudo = 100; p.habilidad = "Curacion Amiga";
    } 
    else if (tipo == 5) {
        p.clase = "Asesino"; p.vida = 100; p.vida_max = 100; p.escudo = 100; p.habilidad = "Golpe Subito";
    } 
    else {
        p.clase = "Personalizado"; p.vida = 100; p.vida_max = 100; p.escudo = 100; p.habilidad = "Modificacion Libre";
    }
    
    return p;
}

/* ==========================================================
4. NODO DEL SISTEMA (Árbol B4)
==========================================================*/
class ArbolB;
void mostrarInventario(NodoItem* cabeza);
struct RegistroOperativo;
void borrar(ArbolB& arbol, int ip_buscar, NodoItem*& itemsCaidos);

struct RegistroOperativo {
    int ip_personaje;
    int ip_bando;
    string nombre_bando;
    Personaje operativo;
};

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
        if (child->cantidad() == t - 1) {
            NodoB* leftSibling = (childIdx > 0) ? hijos[childIdx - 1] : nullptr;
            NodoB* rightSibling = (childIdx < n) ? hijos[childIdx + 1] : nullptr;
            if (leftSibling != nullptr && leftSibling->cantidad() >= t) {
                for (int i = child->n; i > 0; --i) child->llaves[i] = child->llaves[i - 1];
                child->llaves[0] = llaves[childIdx - 1];
                llaves[childIdx - 1] = leftSibling->llaves[leftSibling->n - 1];
                if (!child->hoja) {
                    child->hijos[1] = child->hijos[0];
                    child->hijos[0] = leftSibling->hijos[leftSibling->n];
                }
                ++child->n;
                --leftSibling->n;
            } else if (rightSibling != nullptr && rightSibling->cantidad() >= t) {
                child->llaves[child->n] = llaves[childIdx];
                llaves[childIdx] = rightSibling->llaves[0];
                for (int i = 0; i < rightSibling->n - 1; ++i) rightSibling->llaves[i] = rightSibling->llaves[i + 1];
                if (!child->hoja) {
                    child->hijos[child->n + 1] = rightSibling->hijos[0];
                    for (int i = 0; i < rightSibling->n; ++i) rightSibling->hijos[i] = rightSibling->hijos[i + 1];
                }
                ++child->n;
                --rightSibling->n;
            } else {
                if (leftSibling != nullptr) {
                    leftSibling->llaves[leftSibling->n] = llaves[childIdx - 1];
                    ++leftSibling->n;
                    for (int i = 0; i < child->n; ++i) leftSibling->llaves[leftSibling->n + i] = child->llaves[i];
                    leftSibling->n += child->n;
                    if (!leftSibling->hoja) {
                        for (int i = 0; i <= child->n; ++i) leftSibling->hijos[leftSibling->n - child->n + i] = child->hijos[i];
                    }
                    for (int i = childIdx - 1; i < n - 1; ++i) llaves[i] = llaves[i + 1];
                    --n;
                    delete child;
                    hijos[childIdx] = leftSibling;
                } else if (rightSibling != nullptr) {
                    child->llaves[child->n] = llaves[childIdx];
                    ++child->n;
                    for (int i = 0; i < rightSibling->n; ++i) child->llaves[child->n + i] = rightSibling->llaves[i];
                    child->n += rightSibling->n;
                    if (!child->hoja) {
                        for (int i = 0; i <= rightSibling->n; ++i) child->hijos[child->n - rightSibling->n + i] = rightSibling->hijos[i];
                    }
                    for (int i = childIdx; i < n - 1; ++i) llaves[i] = llaves[i + 1];
                    --n;
                    delete rightSibling;
                    hijos[childIdx + 1] = nullptr;
                }
            }
        }
        return child->eliminarRec(k);
    }

    void recorrerRec() {
        for (int i = 0; i < n; ++i) {
            if (!hoja) hijos[i]->recorrerRec();
            RegistroOperativo& op = llaves[i];
            cout << "[IP: " << op.ip_personaje << " | " << op.nombre_bando
                 << " | " << op.operativo.clase << " | HP: " << op.operativo.vida
                 << " | SHIELD: " << op.operativo.escudo;
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
        bool ok = false;
        if (raiz != nullptr) {
            ok = raiz->eliminarRec(k);
            if (raiz->cantidad() == 0 && raiz->esHoja() == false) {
                NodoB* old = raiz;
                raiz = raiz->hijo(0);
                delete old;
            }
        }
        return ok;
    }

    void recorrer() { if (raiz != nullptr) raiz->recorrerRec(); }

    void limpiar() {
        if (raiz != nullptr) {
            raiz->limpiarRec();
            delete raiz;
            raiz = nullptr;
        }
    }
};

/*==========================================================
5. FUNCIONES DE VALIDACIÓN Y AUXILIARES
==========================================================*/

bool esNombreValido(const string& texto) {
    if (texto.empty()) return false;
    for (char c : texto) {
        if (isdigit(c)) return false; 
    }
    return true;
}

bool existeIP(ArbolB& arbol, int ip_buscar) {
    return buscarOperativoPorIP(arbol, ip_buscar) != nullptr;
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
    if (valor == 1) return "Acción de creación/inyección";
    if (valor == 2) return "Acción de ataque/disparo";
    return "Acción de apoyo/habilidad/recarga";
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
    for (int i = 0; i < tamBase; i++) {
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
    for (int i = 0; i < tamBase; i++) {
        if (arsenalBase[i].id_arma == id_buscar) return &arsenalBase[i];
    }
    NodoArma* aux = listaArmasCustom;
    while (aux != nullptr) {
        if (aux->info_arma.id_arma == id_buscar) return &(aux->info_arma);
        aux = aux->siguiente;
    }
    return nullptr; 
}

RegistroOperativo* buscarOperativoPorIP(ArbolB& arbol, int ip_buscar) {
    return arbol.buscar(ip_buscar); 
}

void mostrarArsenalCompleto(Arma arsenalBase[], int tamBase, NodoArma* listaArmasCustom) {
    cout << "\n--- ARSENAL COMPLETO EN RED ---" << endl;
    for (int i = 0; i < tamBase; i++) {
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

void agregarItem(NodoItem* &inventario, string nombre, string tipoEfecto, int valor) {
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
        const EspecieInstruccion& especie = especiesInstruccion[opcion - 1];
        operativo.clase = especie.nombre;
        operativo.vida = especie.salud;
        operativo.vida_max = especie.salud;
        operativo.escudo = especie.defensa;
        operativo.habilidad = "Plantilla de Especie";
        return true;
    }
    if (categoria == 2) {
        if (opcion < 1 || opcion > 4) return false;
        const HeroeInstruccion& heroe = heroesInstruccion[opcion - 1];
        operativo.clase = heroe.nombre;
        operativo.vida = heroe.salud;
        operativo.vida_max = heroe.salud;
        operativo.escudo = heroe.fortaleza / 2;
        operativo.habilidad = "Habilidad de Heroe";
        return true;
    }
    if (categoria == 3) {
        if (opcion < 1 || opcion > 5) return false;
        const PerfilInstruccion& perfil = personajesInstruccion[opcion - 1];
        operativo.clase = perfil.nombre;
        operativo.vida = perfil.salud;
        operativo.vida_max = perfil.salud;
        operativo.escudo = perfil.fortaleza / 2;
        operativo.habilidad = "Perfil de Instrucción";
        return true;
    }
    return false;
}

/*==========================================================
7. INICIALIZACIÓN DE LÍDERES ÚNICOS
==========================================================*/
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
    liderNeon.operativo.arma_assigned = &arsenalBase[0];
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
    liderOmega.operativo.arma_assigned = &arsenalBase[4];
    liderOmega.operativo.inventario = nullptr;
    agregarItem(liderOmega.operativo.inventario, "Tarjeta de Acceso Alfa", "Escudo", 150);
    arbol.insertar(liderOmega);
    contadorOmega = 1;
}

/*==========================================================
8. MECÁNICAS DE BALAS Y DISPARO AUTOMATIZADO
==========================================================*/
RegistroOperativo* buscarOperativoPorIP(ArbolB& arbol, int ip_buscar);

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

bool ejecutarDisparo(ArbolB& arbol, NodoItem* &itemsCaidos) {
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
        cout << "[¡CLICK!] El arma '" << arma->nombre << "' no tiene balas. Requiere RECUPERAR MUNICION (Opción 9).\n";
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
            cout << "-> El escudo colapso. Absorbió " << objetivo->operativo.escudo << " de daño.\n";
            objetivo->operativo.escudo = 0;
        }
    }

    if (danoProvocado > 0) {
        objetivo->operativo.vida -= danoProvocado;
        if (objetivo->operativo.vida < 0) objetivo->operativo.vida = 0;
        cout << "-> La integridad del objetivo sufrio daños directos.\n";
    }

    cout << ">> Estatus del Objetivo (IP: " << ipObjetivo << ") -> HP: " << objetivo->operativo.vida << " | SHIELD: " << objetivo->operativo.escudo << "\n";

    if (objetivo->operativo.vida <= 0) {
        cout << "\n[¡ELIMINACIÓN CRÍTICA!] El operativo IP " << ipObjetivo << " ha caido en batalla.\n";
        cout << "-> Soltando armamento e inventario al suelo...\n";
        borrar(arbol, ipObjetivo, itemsCaidos);
    }

    return true;
}

void ejecutarRecarga(ArbolB& arbol) {
    cout << "\n--- RECARGA DE ENERGIA/MUNICION ---\n";
    cout << "Ingrese IP del Operativo a recargar: ";
    int ipOp = leerEnteroSeguro();

    RegistroOperativo* op = buscarOperativoPorIP(arbol, ipOp);
    if (!op) {
        cout << "[Error] El operativo no existe.\n";
        return;
    }

    if (op->ip_bando != turnoActual) {
        cout << "[ERROR] Este operativo pertenece al bando enemigo.\n";
        return;
    }

    if (op->operativo.arma_assigned == nullptr) {
        cout << "[Aviso] Este operativo no cuenta con armas en su ranura.\n";
        return;
    }

    Arma* arma = op->operativo.arma_assigned;
    if (arma->bullets_actuales == arma->cargador_maximo) {
        cout << "[Aviso] El cargador de '" << arma->nombre << "' ya esta lleno ("
             << arma->bullets_actuales << "/" << arma->cargador_maximo << "). No es necesario recargar.\n";
        return;
    }

    arma->bullets_actuales = arma->cargador_maximo;
    cout << "-> Cargador reabastecido. '" << arma->nombre << "' lista con " << arma->bullets_actuales << "/" << arma->cargador_maximo << " balas.\n";
}

/*==========================================================
9. NUEVA MECÁNICA: USO DE HABILIDADES PROPIAS DE CADA CLASE
==========================================================*/
void ejecutarHabilidad(ArbolB& arbol, NodoItem* &itemsCaidos) {
    cout << "\n--- ACTIVACION DE HABILIDAD ESPECIAL ---\n";
    cout << "Ingrese IP del Operativo que usara su habilidad: ";
    int ipOp = leerEnteroSeguro();

    RegistroOperativo* ejecutor = buscarOperativoPorIP(arbol, ipOp);
    if (!ejecutor) {
        cout << "[Error] El operativo no existe en la red.\n";
        return;
    }
    if (ejecutor->ip_bando != turnoActual) {
        cout << "[ERROR] Este operativo pertenece al bando enemigo.\n";
        return;
    }
    if (ejecutor->operativo.vida <= 0) {
        cout << "[Error] Un operativo desmantelado no puede usar habilidades.\n";
        return;
    }

    string clase = ejecutor->operativo.clase;
    string hab = ejecutor->operativo.habilidad;
    cout << "-> Operativo de Clase '" << clase << "' activando: [" << hab << "]\n";

    if (hab == "Fuerza Bruta" || clase == "Juggernaut") {
        ejecutor->operativo.escudo += 50;
        if (ejecutor->operativo.escudo > MAX_ESCUDO) ejecutor->operativo.escudo = MAX_ESCUDO;
        cout << "[!] Escudo de plasma sobrecargado. Nuevo Escudo: " << ejecutor->operativo.escudo << "/" << MAX_ESCUDO << "\n";
    }
    else if (hab == "Sigilo Optico" || clase == "Espectro") {
        if (ejecutor->operativo.arma_assigned != nullptr) {
            ejecutor->operativo.arma_assigned->bullets_actuales = ejecutor->operativo.arma_assigned->cargador_maximo;
            cout << "[!] Hackeo de cargador exitoso. '" << ejecutor->operativo.arma_assigned->nombre << "' rellenada al instante.\n";
        } else {
            cout << "[Aviso] El espectro no tiene un arma equipada para rellenar municion.\n";
        }
    }
    else if (hab == "Control de Mentes" || clase == "Hacker") {
        cout << "Ingrese IP del Objetivo Enemigo para robar datos (balas): ";
        int ipObj = leerEnteroSeguro();
        RegistroOperativo* objetivo = buscarOperativoPorIP(arbol, ipObj);

        if (!objetivo || objetivo->ip_bando == ejecutor->ip_bando || ipObj == ipOp) {
            cout << "[Aviso] Objetivo invalido o es un aliado.\n";
            return;
        }
        if (ejecutor->operativo.arma_assigned == nullptr || objetivo->operativo.arma_assigned == nullptr) {
            cout << "[Aviso] Uno de los dos operativos involucrados no posee armas.\n";
            return;
        }

        int balasRobadas = min(5, objetivo->operativo.arma_assigned->bullets_actuales);
        objetivo->operativo.arma_assigned->bullets_actuales -= balasRobadas;
        ejecutor->operativo.arma_assigned->bullets_actuales += balasRobadas;
        if (ejecutor->operativo.arma_assigned->bullets_actuales > ejecutor->operativo.arma_assigned->cargador_maximo) {
            ejecutor->operativo.arma_assigned->bullets_actuales = ejecutor->operativo.arma_assigned->cargador_maximo;
        }
        cout << "[!] Exito. Has hackeado el arma del rival, extrayendo " << balasRobadas << " balas directamente a tu cargador.\n";
    }
    else if (hab == "Curacion Amiga" || clase == "Sacerdote") {
        cout << "Ingrese IP del Operativo Aliado a quien deseas curar: ";
        int ipAliado = leerEnteroSeguro();
        RegistroOperativo* aliado = buscarOperativoPorIP(arbol, ipAliado);

        if (!aliado || aliado->ip_bando != ejecutor->ip_bando || ipAliado == ipOp) {
            cout << "[Aviso] No puedes curarte a ti mismo ni a un rival. Selecciona un aliado real.\n";
            return;
        }
        if (aliado->operativo.vida <= 0) {
            cout << "[Aviso] No puedes revivir operativos completamente desmantelados (0 HP).\n";
            return;
        }

        aliado->operativo.vida += 50;
        if (aliado->operativo.vida > aliado->operativo.vida_max) {
            aliado->operativo.vida = aliado->operativo.vida_max;
        }
        cout << "[!] Inyeccion de nanobots completada. Aliado IP " << ipAliado << " recuperado a " << aliado->operativo.vida << " HP.\n";
    }
    else if (hab == "Golpe Subito" || clase == "Asesino") {
        cout << "Ingrese IP del Objetivo Enemigo a ejecutar: ";
        int ipObj = leerEnteroSeguro();
        RegistroOperativo* objetivo = buscarOperativoPorIP(arbol, ipObj);

        if (!objetivo || objetivo->ip_bando == ejecutor->ip_bando || ipObj == ipOp) {
            cout << "[Aviso] No puedes atacar a tus aliados.\n";
            return;
        }
        if (objetivo->operativo.vida <= 0) {
            cout << "[Aviso] El objetivo ya esta destruido.\n";
            return;
        }

        objetivo->operativo.vida -= 40;
        if (objetivo->operativo.vida < 0) objetivo->operativo.vida = 0;
        cout << "[!] ¡FILTRACIÓN CRÍTICA! Golpe sorpresa inflige 40 de daño evadiendo sus defensas de escudo.\n";
        cout << ">> Estatus del Rival (IP: " << ipObj << ") -> HP: " << objetivo->operativo.vida << " | SHIELD: " << objetivo->operativo.escudo << "\n";

        if (objetivo->operativo.vida <= 0) {
            cout << "\n[¡ELIMINACIÓN CRÍTICA!] El golpe subito destruyo al operativo IP " << ipObj << ".\n";
            borrar(arbol, ipObj, itemsCaidos);
        }
    }
    else {
        cout << "[Aviso] Esta clase no tiene un modulo de accion programado o es de Modificacion Libre.\n";
    }
}

/*==========================================================
10. GESTIÓN CRUD ORIGINAL 
==========================================================*/
void crear(ArbolB& arbol, Arma arsenalBase[], int tamBase, NodoArma* &listaArmasCustom, NodoItem* &itemsCaidos) {
    int bando_sel = turnoActual;
    if ((bando_sel == 1 && contadorNeon >= MAX_MIEMBROS_BANDO) || (bando_sel == 2 && contadorOmega >= MAX_MIEMBROS_BANDO)) {
        cout << "[ERROR CRÍTICO] Servidor lleno.\n"; return;
    }

    cout << "\n--- REGISTRO DE OPERATIVO ---\nIP Unica: "; 
    int ip_p = leerEnteroSeguro();
    if (ip_p == 10 || ip_p == 20) { cout << "[Error] IPs 10 y 20 reservadas.\n"; return; }
    if (arbol.buscar(ip_p) != nullptr) { cout << "[Error] IP ya existe.\n"; return; }

    RegistroOperativo nuevoOp;
    nuevoOp.ip_personaje = ip_p;
    if (bando_sel == 1) { nuevoOp.ip_bando = 1; nuevoOp.nombre_bando = "Resistencia Neon"; contadorNeon++; } 
    else { nuevoOp.ip_bando = 2; nuevoOp.nombre_bando = "Corporacion OMEGA"; contadorOmega++; }

    cout << "Clase (1: Juggernaut | 2: Espectro | 3: Hacker | 4: Healer | 5: Asesino): "; 
    int tipo = leerEnteroSeguro();
    nuevoOp.operativo = obtenerPlantillaPersonaje(tipo);
    nuevoOp.operativo.inventario = nullptr;

    // Asignación de arma simple para ahorrar espacio en la demostración
    cout << "Ingrese ID Arma base (ej. 101 a 110): ";
    int id_arma_sel = leerEnteroSeguro();
    nuevoOp.operativo.arma_assigned = buscarArmaPorID(arsenalBase, tamBase, listaArmasCustom, id_arma_sel);

    arbol.insertar(nuevoOp);
    cout << "-> Operativo inyectado al Arbol B4 con exito.\n";
}

void leer(ArbolB& arbol) {
    cout << "\n=== STATUS SERVIDORES ===\n";
    cout << "Neon: " << contadorNeon << " | Omega: " << contadorOmega << "\n--------------------\n";
    arbol.recorrer();
}

void actualizar(ArbolB& arbol, int ip_buscar) {
    if (ip_buscar == 10 || ip_buscar == 20) {
        cout << "-> Acceso denegado: Los estatus de los Lideres de Bando no pueden ser alterados.\n";
        return;
    }

    RegistroOperativo* op = buscarOperativoPorIP(arbol, ip_buscar);
    if (!op) {
        cout << "-> IP no localizada.\n";
        return;
    }

    cout << "\nOperativo encontrado. Modificando IP: " << ip_buscar << endl;
    cout << "Ingrese nueva Vida (HP - Maximo " << op->operativo.vida_max << "): ";
    int nuevaVida = leerEnteroSeguro();
    if (nuevaVida > op->operativo.vida_max) {
        cout << "[Aviso] Modificacion excedida. Seteada al maximo de " << op->operativo.vida_max << " HP.\n";
        op->operativo.vida = op->operativo.vida_max;
    } else {
        op->operativo.vida = nuevaVida;
    }

    cout << "Ingrese nuevo Escudo (SHIELD - Maximo " << MAX_ESCUDO << "): ";
    int nuevoEscudo = leerEnteroSeguro();
    if (nuevoEscudo > MAX_ESCUDO) {
        cout << "[Aviso] Modificacion excedida. Seteada al maximo de " << MAX_ESCUDO << " SHIELD.\n";
        op->operativo.escudo = MAX_ESCUDO;
    } else {
        op->operativo.escudo = nuevoEscudo;
    }

    cout << "-> Datos actualizados correctamente.\n";
}

void borrar_todo(ArbolB& arbol) {
    arbol.limpiar();
    contadorNeon = 0;
    contadorOmega = 0;
}

void borrar_armas_custom(NodoArma* &inicio) {
    NodoArma* aux;
    while (inicio != nullptr) {
        aux = inicio;
        inicio = inicio->siguiente;
        delete aux;
    }
}

void borrar_items_caidos(NodoItem* &inicio) {
    NodoItem* aux;
    while (inicio != nullptr) {
        aux = inicio;
        inicio = inicio->siguiente;
        delete aux;
    }
}

void ejecutarUsoItemVida(ArbolB& arbol) {
    cout << "\n--- PROTOCOLO DE REGENERACIÓN DE BIOMASA (ITEMS) ---\n";
    cout << "Ingrese IP del Operativo que usara el item: ";
    int ipOp = leerEnteroSeguro();

    RegistroOperativo* op = buscarOperativoPorIP(arbol, ipOp);
    if (!op) {
        cout << "[Error] El operativo no existe en la red.\n";
        return;
    }
    if (op->ip_bando != turnoActual) {
        cout << "[ERROR] Este operativo pertenece al bando enemigo.\n";
        return;
    }
    if (op->operativo.vida <= 0) {
        cout << "[Error] Un operativo desmantelado no puede consumir objetos.\n";
        return;
    }
    if (op->operativo.inventario == nullptr) {
        cout << "[Aviso] El inventario de este operativo esta vacio.\n";
        return;
    }

    NodoItem* actual = op->operativo.inventario;
    NodoItem* anterior = nullptr;
    bool itemEncontrado = false;

    while (actual != nullptr) {
        if (actual->tipo_efecto == "Vida") {
            itemEncontrado = true;
            break;
        }
        anterior = actual;
        actual = actual->siguiente;
    }

    if (!itemEncontrado) {
        cout << "[Aviso] El operativo no tiene ningun item de tipo 'Vida' en su inventario.\n";
        return;
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
}

/*========================================================
11. MODO DE JUEGO ALEATORIO
==========================================================*/
string nombreBandoAleatorio(int bando) {
    return (bando == 1) ? "Neon" : "Omega";
}

int lanzarDadoTresCaras() {
    return 1 + rand() % 3;
}


/*========================================================
12. INTRODUCCIÓN HISTÓRICA
==========================================================*/
void mostrarIntroduccion() {
    cout << "========================================================================\n";
    cout << "                    AÑO 2099 - NEO-KOREA (RED SUBTERRÁNEA)             \n";
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

/*========================================================
13. MENÚ PRINCIPAL CON LA OPCIÓN DE HABILIDADES
==========================================================*/
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
        }
        else if (seleccionInicio != 1) {
            cout << "[Error] Codigo de comando invalido. Intente de nuevo.\n\n";
        }
    }

    cout << "\n Connecting... [OK] \n Cargando protocolos de red corporativos y rebeldes...\n";

    ArbolB yggdrasil(2);
    NodoArma* arsenalCustom = nullptr; 
    NodoItem* sueloItemsCaidos = nullptr; 
    
    int opcion = -1; 
    int id_busqueda;
    const int TAM_BASE = 10;

    Arma arsenalBase[TAM_BASE] = {
        {101, "Canon de Riel", 45, 5, 5},
        {102, "Daga de datos", 15, 30, 30},
        {103, "Virus de Enlace", 35, 10, 10},
        {104, "Varita de Sauco", 10, 50, 50},
        {105, "Cuchillas sangrientas", 55, 12, 12},
        {106, "Cañón de Plasma Pesado", 180, 50, 50},
        {107, "Rifle Francotirador Gauss", 250, 120, 120},
        {108, "Espada de Iones", 130, 200, 2},
        {109, "Granada de Pulso Electromagnético", 90, 10, 30},
        {110, "Inyector de Malware", 60, 300, 15}
    };

    cargarLideresPredeterminados(yggdrasil, arsenalBase);
    turnoActual = (rand() % 2 == 0) ? 1 : 2;
    accionesRestantes = 1 + rand() % 3;

    cout << "\n[!] Conexión establecida. El primer turno es para: " << nombreDelTurnoActual() << "\n";
    cout << "[!] Dado inicial lanzado: Tienes " << accionesRestantes << " acciones disponibles.\n";

    while (opcion != 0) {
        if (accionesRestantes <= 0) {
            avanzarTurno();
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
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEnteroSeguro();

        if (opcion == 1) {
            crear(yggdrasil, arsenalBase, TAM_BASE, arsenalCustom, sueloItemsCaidos);
            accionesRestantes--;
        } 
        else if (opcion == 2) {
            leer(yggdrasil);
        } 
        else if (opcion == 3) {
            cout << "Borrando todo el ordenador cuantico...\n";
            borrar_todo(yggdrasil);
            borrar_items_caidos(sueloItemsCaidos);
            cargarLideresPredeterminados(yggdrasil, arsenalBase);
            cout << "-> Barrido completo. Servidores reiniciados al estado nativo.\n";
            accionesRestantes = 1 + rand() % 3;
        } 
        else if (opcion == 4) {
            mostrarArsenalCompleto(arsenalBase, TAM_BASE, arsenalCustom);
        } 
        else if (opcion == 5) {
            cout << "\n--- ITEMS EN EL SUELO DESATADOS ---\n";
            mostrarInventario(sueloItemsCaidos);
            accionesRestantes--;
            cout << "\n";
        } 
        else if (opcion == 6) {
            if (ejecutarDisparo(yggdrasil, sueloItemsCaidos)) {
                accionesRestantes--; 
            }
        }
        else if (opcion == 7) {
            ejecutarRecarga(yggdrasil);
            accionesRestantes--;
        }
        else if (opcion == 8) {
            ejecutarHabilidad(yggdrasil, sueloItemsCaidos);
            accionesRestantes--;
        }
        else if (opcion == 9){
            ejecutarUsoItemVida(yggdrasil);
            accionesRestantes--;
        }
        else if (opcion == 0) {
            cout << "Saliendo del sistema Yggdrasil.\n";
        } 
        else {
            cout << "Opcion no valida.\n";
        }
    } 

    borrar_todo(yggdrasil);
    borrar_armas_custom(arsenalCustom);
    borrar_items_caidos(sueloItemsCaidos);
    return 0;
}
