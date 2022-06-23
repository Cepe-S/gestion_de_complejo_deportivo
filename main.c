#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <conio.h>
#include <unistd.h>
#include <windows.h>

#define UP_KEY 72
#define DOWN_KEY 80
#define LEFT_KEY 75
#define RIGHT_KEY 77
#define ENTER_KEY 13
#define X_KEY 120

#define DEFAULT_COLOR 7
#define HOVER_COLOR 127
#define SELECTED_COLOR 14

#define SET_COLOR_RED     "\x1b[31m"
#define SET_COLOR_YELLOW  "\x1b[33m"
#define SET_COLOR_GREEN   "\x1b[32m"
#define SET_COLOR_RESET   "\x1b[0m"

int back_color(int num) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), num);
    return 0;
}

// borra todo lo que está en el stdout
void clrscr() {
    system("@cls||clear");
}

// esconde el cursor
void hidecursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// traba el sistema hasta que se aprete enter
void volver() {
    bool run = true;
    back_color(HOVER_COLOR);
    printf("Volver");
    back_color(DEFAULT_COLOR);
    while (run) {
        if (getch() == 13) {
            run = false;
        }
    }
    clrscr();
}

// TODO: mover funciones a librerias

typedef struct {
    char tipo; // f: futbol, b: basket, t: tenis, p: padel, s: squash
    int precio; // por 2 horas
    int capacidad;
} tipo_cancha;

typedef struct {
    int id;
    char tipo;
} cancha;

typedef struct {
    char nombreyapellido[30];
    int dni; // id único
    long int telefono;
} cliente;

// un turno = 2 horas
typedef struct {
    int id_turno; // 1 = 10 a 12, 2 = 12 a 14, 3 = 14 a 16, 4 = 16 a 18, 5 = 18 a 20, 6 = 20 a 22
    int dia;
    int mes;
    int anio;
    int cancha_id;
    int cliente_id;
} turno;

void start_turnosxdia(bool *d) {
    d[0] = true;
    d[1] = true;
    d[2] = true;
    d[3] = true;
    d[4] = true;
    d[5] = true;
}

bool* turnos_disponibles(int dia, int mes, int anio, int cancha) {
    FILE *archivo;
    archivo = fopen("turnos.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    turno t[size];
    fread(&t, sizeof(turno), size, archivo);
    fclose(archivo);
    bool* d = (bool*) malloc(sizeof(bool) * 6);
    start_turnosxdia(d);
    for (int i = 0; i < size / sizeof(turno); i++) {
        if (t[i].dia == dia && t[i].mes == mes && t[i].cancha_id == cancha) {
            d[t[i].id_turno - 1] = false;
        }
    }
    return d;
}

char* get_nombre_cancha(char tipo) {
    switch (tipo) {
        case 'f':
            return "Futbol";
        case 'b':
            return "Basket";
        case 't':
            return "Tenis";
        case 'p':
            return "Padel";
        case 's':
            return "Squash";
        default:
            return "";
    }
}

cancha get_cancha_by_id(int id) {
    FILE *archivo;
    archivo = fopen("canchas.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    cancha c[size];
    fread(&c, sizeof(cancha), size, archivo);
    fclose(archivo);
    for (int i = 0; i < size / sizeof(cancha); i++) {
        if (c[i].id == id) {
            return c[i];
        }
    }
    return c[0];
}

int get_precio_cancha(char tipo) {
    FILE *archivo = fopen("canchas_info.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    tipo_cancha tc[size];
    fread(&tc, sizeof(tipo_cancha), size, archivo);
    fclose(archivo);
    for (int i = 0; i < size / sizeof(tipo_cancha); i++) {
        if (tc[i].tipo == tipo) {
            return tc[i].precio;
        }
    }
}

// busca un cliente en el archivo con su dni y lo devuelve
cliente find_cliente_by_dni(int dni) {
    FILE *archivo;
    archivo = fopen("clientes.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    cliente c[size];
    fread(&c, sizeof(cliente), size, archivo);
    fclose(archivo);
    for (int i = 0; i < size; i++) {
        if (c[i].dni == dni) {
            return c[i]; // esta funcion se utiliza solo después de check_dni
        }
    }
}

char* turno_id_to_str(int t_id) {
    if (t_id = 1) {
        return "10:00 a 12:00";
    } else if (t_id = 2) {
        return "12:00 a 14:00";
    } else if (t_id = 3) {
        return "14:00 a 16:00";
    } else if (t_id = 4) {
        return "16:00 a 18:00";
    } else if (t_id = 5) {
        return "18:00 a 20:00";
    } else if (t_id = 6) {
        return "20:00 a 22:00";
    }
}

// crea un recibo de pago
void make_recibo(turno t) {
    char filename[30];
    sprintf(filename, "%s\\%d_%d..%d.%d.%d %d.txt", "recibos", t.cliente_id, t.id_turno, t.dia, t.mes, t.anio, t.cancha_id);
    FILE *f = fopen(filename, "a");
    cliente c = find_cliente_by_dni(t.cliente_id);
    cancha ca = get_cancha_by_id(t.cancha_id);
    fprintf(f, "----Recibo de turno----\n\n");
    fprintf(f, "Fecha: %d/%d/%d\n", t.dia, t.mes, t.anio);
    fprintf(f, "Turno: %s\n", turno_id_to_str(t.id_turno));
    fprintf(f, "Cancha: %s id %d\n", get_nombre_cancha(ca.tipo), t.cancha_id);
    fprintf(f, "------------------------\n");
    fprintf(f, "Cliente: %s\n", c.nombreyapellido);
    fprintf(f, "DNI: %d\n", t.cliente_id);
    fprintf(f, "Telefono: 223%d\n", c.telefono);
    fprintf(f, "------------------------\n");
    fprintf(f, "Precio: $%.2d\n", get_precio_cancha(ca.tipo));
    fclose(f);
}

// devuelve las ganancias en un determinado tiempo de un tipo de cancha
int get_ganancias_cancha(char tipo, int tiempo) {
    FILE *archivo = fopen("turnos.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    turno t[size];
    fread(&t, sizeof(turno), size, archivo);
    fclose(archivo);
    int precio = get_precio_cancha(tipo);
    int total = 0;
    SYSTEMTIME f;
    GetSystemTime(&f);
    if (tiempo == 1) {
        for (int i = 0; i < size / sizeof(turno); i++) {
            if (get_cancha_by_id(t[i].cancha_id).tipo == tipo && t[i].dia == f.wDay) {
                total = total + precio;
            }
        }
    }
    if (tiempo == 2) {
        for (int i = 0; i < size / sizeof(turno); i++) {
            if (get_cancha_by_id(t[i].cancha_id).tipo == tipo && t[i].mes == f.wMonth) {
                total = total + precio;
            }
        }
    }
    if (tiempo == 3) {
        for (int i = 0; i < size / sizeof(turno); i++) {
            if (get_cancha_by_id(t[i].cancha_id).tipo == tipo && t[i].anio == f.wYear) {
                total = total + precio;
            }
        }
    }
    return total;
}

// transforma una flag de tiempo a string
char *tiempo_to_str(int tiempo) {
    switch (tiempo) {
        case 1:
            return "Dia";
        case 2:
            return "Mes";
        case 3:
            return "Anio";
        default:
            return "";
    }
}

// muestra las ganancias en un determinado tiempo
void show_ganancias(int tiempo) {
    clrscr();
    char tipos[5] = {'f', 'b', 't', 'p', 's'};
    int ganancias;
    char tiempo_str[10];
    int total = 0;
    strcpy(tiempo_str, tiempo_to_str(tiempo));
    printf("<Ganancias del %s>\n", tiempo_str);
    for (int i = 0; i < 5; i++) {
        ganancias = get_ganancias_cancha(tipos[i], tiempo);
        char tipo = tipos[i];
        if (tiempo == 1) {
            ganancias = get_ganancias_cancha(tipo, 1);
        }
        if (tiempo == 2) {
            ganancias = get_ganancias_cancha(tipo, 2);
        }
        if (tiempo == 3) {
            ganancias = get_ganancias_cancha(tipo, 3);
        }
        printf("Canchas de %s: $%d\n", get_nombre_cancha(tipo), ganancias);
        total = total + ganancias;
    }
    printf("\nGanancias totales: $%d\n", total);
    volver();
}

// muestra un cliente
void print_cliente(cliente c) {
    printf("Nombre y apellido: %s\n", c.nombreyapellido);
    printf("DNI: %d\n", c.dni);
    printf("Telefono: 223%d\n", c.telefono);
}

// muestra un turno
void print_turno(turno t) {
    printf("Id del turno: %d\n", t.id_turno);
    printf("Fecha: %d/%d/%d\n", t.dia, t.mes, t.anio);
    printf("Id de la cancha: %d\n", t.cancha_id);
    printf("DNI del cliente: %d\n", t.cliente_id);
}

// muestra una cancha
void print_cancha(cancha c) {
    printf("Tipo de cancha: %s\n", get_nombre_cancha(c.tipo));
    printf("Id de la cancha: %d\n", c.id);
}

// muestra el cliente y pide la confirmación del usuario para guardarlo
bool confirmar_cliente(cliente c) {
    bool i = true;
    bool run = true;
    while (run) {
        printf("--- Cliente creado ---\n\n");
        print_cliente(c); printf("\n");
        if (i) {
            back_color(HOVER_COLOR);
            printf("Confirmar\n");
            back_color(DEFAULT_COLOR);
            printf("Editar datos\n");
        }
        if (!i) {
            printf("Confirmar\n");
            back_color(HOVER_COLOR);
            printf("Editar datos");
            back_color(DEFAULT_COLOR);
        }
        int c = getch();
        if (c == UP_KEY || c == DOWN_KEY) {
            i = !i;
        }
        if (c == 13) {
            return i;
        }
        clrscr();
    }
}

// muestra el turno y pide la confirmación del usuario para guardarlo
bool confirmar_turno(turno t) {
    bool i = true;
    bool run = true;
    while (run) {
        printf("--- Turno creado ---\n\n");
        print_turno(t);
        printf("\nTotal a pagar: $%d\n\n", get_precio_cancha(get_cancha_by_id(t.cancha_id).tipo));
        if (i) {
            back_color(HOVER_COLOR);
            printf("Confirmar\n");
            back_color(DEFAULT_COLOR);
            printf("Editar datos\n");
        }
        if (!i) {
            printf("Confirmar\n");
            back_color(HOVER_COLOR);
            printf("Editar datos\n");
            back_color(DEFAULT_COLOR);
        }
        int c = getch();
        if (c == UP_KEY || c == DOWN_KEY) {
            i = !i;
        }
        if (c == 13) {
            return i;
        }
        clrscr();
    }
}

// muestra la cancha y pide la confirmación del usuario para guardarla
bool confirmar_cancha(cancha c) {
    bool i = true;
    bool run = true;
    while (run) {
        printf("--- Cancha creada ---\n\n");
        print_cancha(c); printf("\n");
        if (i) {
            back_color(HOVER_COLOR);
            printf("Confirmar\n");
            back_color(DEFAULT_COLOR);
            printf("Cancelar\n");
        }
        if (!i) {
            printf("Confirmar\n");
            back_color(HOVER_COLOR);
            printf("Cancelar\n");
            back_color(DEFAULT_COLOR);
        }
        int c = getch();
        if (c == UP_KEY || c == DOWN_KEY) {
            i = !i;
        }
        if (c == ENTER_KEY) {
            return i;
        }
        clrscr();
    }
}

// loads an tipo_cancha struct from console and returns it
tipo_cancha crear_tipo_cancha() {
    tipo_cancha t;
    printf("Ingrese el tipo de cancha: ");
    fflush(stdin);
    scanf("%c", &t.tipo);
    printf("Ingrese el precio: ");
    scanf("%d", &t.precio);
    printf("Ingrese la capacidad: ");
    scanf("%d", &t.capacidad);
    return t;
}

// opens a file and saves canchas in an array
void cargar_tipos_de_canchas() {
    FILE *archivo;
    tipo_cancha c;
    for (int i = 0; i < 5; i++) {
        archivo = fopen("canchas_info.dat", "ab");
        fflush(stdin);
        c = crear_tipo_cancha();
        fwrite(&c, sizeof(tipo_cancha), 1, archivo);
        fclose(archivo);
    }
}

// prints the file canchas.dat in the console
void mostrar_tipos_de_canchas() {
    FILE *archivo;
    archivo = fopen("canchas_info.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    tipo_cancha c[size];
    fread(&c, sizeof(cancha), size, archivo);
    for (int i = 0; i < 5; i++) {
        printf("Tipo de cancha: %s\n", get_nombre_cancha(c[i].tipo));
        printf("Precio de la cancha: %d\n", c[i].precio);
        printf("Capacidad de la cancha: %d\n", c[i].capacidad);
    }
    fclose(archivo);
}

// devuelve la cantidad de canchas
int get_cantidad_de_canchas() {
    FILE *archivo;
    archivo = fopen("canchas.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    cancha c[size];
    fread(&c, sizeof(cancha), size, archivo);
    fclose(archivo);
    return size / sizeof(cancha);
}

// loads a cancha struct from console and returns it
cancha crear_cancha() {
    cancha c;
    printf("Ingrese el tipo de cancha\n");
    printf("[1] Futbol\n");
    printf("[2] Basquet\n");
    printf("[3] Tenis\n");
    printf("[4] Padel\n");
    printf("[5] Squash\n");
    int tipo;
    scanf("%d", &tipo);
    if (tipo < 1 || tipo > 5) {
        printf("Tipo de cancha no válido\n");
        while(tipo < 1 || tipo > 5) {
            printf("Ingrese un tipo de cancha valido: ");
            scanf("%d", &tipo);
        }
    }
    switch (tipo) {
        case 1:
            c.tipo = 'f';
            break;
        case 2:
            c.tipo = 'b';
            break;
        case 3:
            c.tipo = 't';
            break;
        case 4:
            c.tipo = 'p';
            break;
        case 5:
            c.tipo = 's';
            break;
    }
    fflush(stdin);
    c.id = get_cantidad_de_canchas() + 1;
    return c;
}

// carga una cancha al archivo
void cargar_cancha() {
    clrscr();
    bool e;
    cancha c = crear_cancha();
    clrscr();
    e = confirmar_cancha(c);
    if (e) {
        FILE *archivo;
        archivo = fopen("canchas.dat", "ab");
        fwrite(&c, sizeof(cancha), 1, archivo);
        fclose(archivo);
    }
    clrscr();
}

// muestra todas las canchas en una lista
void mostrar_canchas() {
    FILE *archivo;
    archivo = fopen("canchas.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    cancha c[size];
    fread(&c, sizeof(cancha), size, archivo);
    fclose(archivo);
    
    for (int i = 0; i < size / sizeof(cancha); i++) {
        print_cancha(c[i]);
    }

}

// devuelve un dni ingresado por el usuario
int get_dni() {
    int dni;
    printf("Ingrese el DNI: ");
    scanf("%d", &dni);
    if (dni < 1000000 || dni > 60000000) {
        while (dni < 1000000 || dni > 60000000) {
            printf("DNI invalido, ingrese un DNI valido: ");
            scanf("%d", &dni);
        }
    }
    return dni;
}

// verifica si un dni esta en el archivo
bool check_dni(int dni) {
    FILE *archivo;
    archivo = fopen("clientes.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    cliente c[size];
    fread(&c, sizeof(cliente), size, archivo);
    fclose(archivo);
    for (int i = 0; i < size; i++) {
        if (c[i].dni == dni) {
            return true;
        }
    }
    return false;
}

// carga clientes solo cuando no estan en el archivo
cliente crear_cliente() {
    cliente c;
    c.dni = get_dni();
    while (check_dni(c.dni)) {
        printf("El DNI ya existe, ingrese un DNI valido: ");
        c.dni = get_dni();
    }
    printf("Ingrese el nombre y apellido: ");
    fflush(stdin);
    scanf("%[^\n]s", &c.nombreyapellido);
    printf("Ingrese el telefono del cliente: 223-");
    fflush(stdin);
    scanf("%d", &c.telefono);
    return c;

}

// carga un cliente pasado por parametro al archivo
void cargar_cliente() {
    int i;
    bool run = true;
    cliente c;
    while (run) {
        clrscr();
        c = crear_cliente();
        clrscr();
        run = !confirmar_cliente(c);
    }
    FILE *archivo;
    archivo = fopen("clientes.dat", "ab");
    fwrite(&c, sizeof(cliente), 1, archivo);
    fclose(archivo);
    printf("Cliente agregado con exito\n");
    sleep(1);
    clrscr();
}

// crea un cliente con su dni pasado por parametro (esta funcion solo se usa con clientes nuevos)
cliente crear_cliente_con_dni(int dni) {
    cliente c;
    c.dni = dni;
    printf("Ingrese el nombre y apellido del cliente: ");
    fflush(stdin);
    scanf("%[^\n]s", &c.nombreyapellido);
    printf("Ingrese el telefono del cliente: 223-");
    fflush(stdin);
    scanf("%d", &c.telefono);
    return c;
}

// carga el cliente (esta funcion solo se usa con clientes nuevos)
void cargar_cliente_con_dni(int dni) {
    int i;
    bool run = true;
    cliente c;
    while (run) {
        clrscr();
        c = crear_cliente_con_dni(dni);
        clrscr();
        run = !confirmar_cliente(c);
    }
    FILE *archivo;
    archivo = fopen("clientes.dat", "ab");
    fwrite(&c, sizeof(cliente), 1, archivo);
    fclose(archivo);
    printf("Cliente agregado con exito\n");
    sleep(1);
    clrscr();
}

// muestra los clientes en formato de tabla
void mostrar_clientes() {
    FILE *archivo;
    archivo = fopen("clientes.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    cliente c[size];
    fread(&c, sizeof(cliente), size, archivo);
    for (int i = 0; i < size / sizeof(cliente); i++) {
        print_cliente(c[i]);
        printf("------------------------------\n");
    }
    fclose(archivo);
}

// crea un turno
turno crear_turno(int dni) {
    turno t;
    printf("Ingrese el id del turno: ");
    scanf("%d", &t.id_turno);
    printf("Ingrese el dia del turno: ");
    scanf("%d", &t.dia);
    printf("Ingrese el mes del turno: ");
    scanf("%d", &t.mes);
    printf("Ingrese el anio del turno: ");
    scanf("%d", &t.anio);
    printf("Ingrese el id de la cancha: ");
    scanf("%d", &t.cancha_id);
    t.cliente_id = dni;
    return t;
}

// agrega un turno al archivo
void write_turno(turno t) {
    FILE *archivo;
    archivo = fopen("turnos.dat", "ab");
    fwrite(&t, sizeof(turno), 1, archivo);
    fclose(archivo);
}

// carga turnos solo cuando el horario no esta ocupado
void cargar_turnos() {
    bool run = true;
    turno t;
    clrscr();
    while (run) {
        printf("Ingrese el DNI del cliente: ");
        int dni;
        scanf("%d", &dni);
        if (!check_dni(dni)) {
            printf("Cliente nuevo!\n");
            cargar_cliente_con_dni(dni);
            sleep(1);
            clrscr();
        }
        t = crear_turno(dni);
        run = !confirmar_turno(t);
    }
    if (turnos_disponibles(t.dia, t.mes, t.anio, t.cancha_id)[t.id_turno-1]) {
        write_turno(t);
        printf("Turno agregado con exito\n");
        sleep(1);
        clrscr();
    } else {
        printf("El turno ya esta ocupado\n");
        sleep(1);
        clrscr();
    }
}

// devuelve un turno con todos los datos pasados por parametro
turno get_turno(int dia, int mes, int anio, int cancha_id, int id_turno) {
    FILE *archivo;
    archivo = fopen("turnos.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    turno t[size];
    fread(&t, sizeof(turno), size, archivo);
    fclose(archivo);
    for (int i = 0; i < size / sizeof(turno); i++) {
        if (t[i].dia == dia && t[i].mes == mes && t[i].anio == anio && t[i].cancha_id == cancha_id && t[i].id_turno == id_turno) {
            return t[i]; // la funcion solo se usa con valores que ya existen en el archivo
        }
    }
}

// muestra los turnos en una lista
void mostrar_turnos() {
    FILE *archivo;
    archivo = fopen("turnos.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    turno t[size];
    fread(&t, sizeof(turno), size, archivo);
    fclose(archivo);
    SYSTEMTIME f;
    GetLocalTime(&f);
    for (int i = 0; i < size / sizeof(turno); i++) {
        if (t[i].dia >= f.wDay && t[i].mes >= f.wMonth && t[i].anio >= f.wYear) {
            print_turno(t[i]);
            printf("------------------------------\n");
        }
    }
    
}

// devuelve los dias de un mes
int dias_del_mes(int mes, int year) {
    // Enero
    if (mes == 0) {
        return (31);
    }
    // Febrero
    if (mes == 1) {
        // detecta si el año es bisiesto
        if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) {
            return (29);
        }
        return (28);
    }
    // Marzo
    if (mes == 2) {
        return (31);
    }
    // Abril
    if (mes == 3) {
        return (30);
    }
    // Mayo
    if (mes == 4) {
        return (31);
    }
    // Junio
    if (mes == 5) {
        return (30);
    }
    // Julio
    if (mes == 6) {
        return (31);
    }
    // Agosto
    if (mes == 7) {
        return (31);
    }
    // Septiembre
    if (mes == 8) {
        return (30);
    }
    // Octubre
    if (mes == 9) {
        return (31);
    }
    // Noviembre
    if (mes == 10) {
        return (30);
    }
    // Diciembre
    if (mes == 11) {
        return (31);
    }
}

// devuelve el nombre de un mes con su numero (0 = Enero, 1 = Febrero, etc)
char* nombre_de_mes(int mes) {
    char* nombre;
    switch (mes) {
    case 0:
        nombre = "Enero";
        break;
    case 1:
        nombre = "Febrero";
        break;
    case 2:
        nombre = "Marzo";
        break;
    case 3:
        nombre = "Abril";
        break;
    case 4:
        nombre = "Mayo";
        break;
    case 5:
        nombre = "Junio";
        break;
    case 6:
        nombre = "Julio";
        break;
    case 7:
        nombre = "Agosto";
        break;
    case 8:
        nombre = "Septiembre";
        break;
    case 9:
        nombre = "Octubre";
        break;
    case 10:
        nombre = "Noviembre";
        break;
    case 11:
        nombre = "Diciembre";
        break;
    }
    return nombre;
}

// devuelve el nombre de un dia de la semana con su numero (0 = Lunes, 1 = Martes, etc)
char* nombre_de_semana(int dia) {
    char* nombre;
    switch (dia) {
    case 0:
        nombre = "Lunes";
        break;
    case 1:
        nombre = "Martes";
        break;
    case 2:
        nombre = "Miercoles";
        break;
    case 3:
        nombre = "Jueves";
        break;
    case 4:
        nombre = "Viernes";
        break;
    case 5:
        nombre = "Sabado";
        break;
    case 6:
        nombre = "Domingo";
        break;
    }
    return nombre;
}

// devuelve el dia de la semana de una fecha
int get_dia_de_semana(int year, int month, int day) {
    int wday;
    wday = (day + ((153 * (month + 12 * ((14 - month) / 12) - 3) + 2) / 5)
               + (365 * (year + 4800 - ((14 - month) / 12)))
               + ((year + 4800 - ((14 - month) / 12)) / 4)
               - ((year + 4800 - ((14 - month) / 12)) / 100)
               + ((year + 4800 - ((14 - month) / 12)) / 400)
               - 32045)
        % 7;
    return wday;
}

// devuelve de que dia debe ser un color en el calendario 1 = todo el dia libre, 2 = dia con turnos, 3 = dia sin espacio
int get_day_color(bool *d) { 
    int j = 0;
    for (int i = 0; i < 6; i++) {
        if (d[i]) {
            j++;
        }
    }
    if (j == 6) {
        return 1;
    } else if (j == 0) {
        return 3;
    } else {
        return 2;
    }
}

// elimina un turno del archivo
void eliminar_turno(int dia, int mes, int anio, int id_turno, int cancha) {
    FILE *archivo = fopen("turnos.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    turno t[size];
    fread(&t, sizeof(turno), size, archivo);
    fclose(archivo);
    FILE *temp = fopen("temp.dat", "wb");
    for (int i = 0; i < size / sizeof(turno); i++) {
        if (t[i].dia != dia || t[i].mes != mes || t[i].anio != anio || t[i].id_turno != id_turno || t[i].cancha_id != cancha) {
            fwrite(&t[i], sizeof(turno), 1, temp);
        }
    }
    fclose(temp);
    remove("turnos.dat");
    rename("temp.dat", "turnos.dat");
}

// agrega un turno con el dia, mes, anio, id_turno, cancha_id pasados por parametro
void crear_turno_sin_cliente(int dia, int mes, int anio, int id_turno, int cancha) {
    int dni;
    FILE *archivo;
    archivo = fopen("turnos.dat", "ab");
    turno t;
    t.dia = dia;
    t.mes = mes;
    t.anio = anio;
    t.id_turno = id_turno;
    t.cancha_id = cancha;
    printf("Introduzca el DNI del cliente: ");
    scanf("%d", &dni);
    t.cliente_id = dni;
    clrscr();
    if (!check_dni(dni)) {
        printf("Cliente nuevo!\n");
        cargar_cliente_con_dni(dni);
    }
    switch(confirmar_turno(t)) {
        case true:
        write_turno(t);
        printf("Turno agregado con exito!\n");
        make_recibo(t);
        break;
        case false:
        break;
    }
}

// crea un menu de 2 opciones y devuelve la seleccionada
int menu_2_opciones(char* opciontrue, char* opcionfalse, bool opcion) {
    if (opcion) {
        back_color(HOVER_COLOR);
        printf("%s\n", opciontrue);
        back_color(DEFAULT_COLOR);
        printf("%s\n", opcionfalse);
    } 
    if (!opcion) {
        printf("%s\n", opciontrue);
        back_color(HOVER_COLOR);
        printf("%s\n", opcionfalse);
        back_color(DEFAULT_COLOR);
    }
    int e = getch();
    if (e == ENTER_KEY) {
        return 1;
    }
    if (e == DOWN_KEY || e == UP_KEY) {
        return 2;
    }
}

// edita un turno, si esta ocupado lo elimina y si esta libre lo crea
void editar_turno(int dia, int mes, int anio, int id_turno, int cancha, bool disponible) {
    if (disponible) {
        bool e = true;
        bool run = true;
        while (run) {
            clrscr();
            printf("Fecha: %d/%d/%d\n", dia, mes, anio);
            printf("Turno: %d\n", id_turno);
            printf("Cancha: %d\n\n", cancha);
            int c = menu_2_opciones("Reservar turno", "Volver", e);
            if (c == 1) {
                if (e) {
                    crear_turno_sin_cliente(dia, mes, anio, id_turno, cancha);
                    run = false;
                    break;
                }
                if (!e) {
                    run = false;
                }
            }
            if (c == 2) {
                e = !e;
            }
            clrscr();
        }
    }
    if (!disponible) {
        bool e = true;
        bool run = true;
        while (run) {
            clrscr();
            printf("Fecha: %d/%d/%d\n", dia, mes, anio);
            printf("Turno: %d\n", id_turno);
            printf("Cancha: %d\n\n", cancha);
            printf("--Cliente--\n");
            print_cliente(find_cliente_by_dni(get_turno(dia, mes, anio, cancha, id_turno).cliente_id));
            printf("\n");
            int c = menu_2_opciones("Cancelar turno", "Volver", e);
            if (c == 1) {
                if (e) {
                    eliminar_turno(dia, mes, anio, id_turno, cancha);
                    printf("Turno cancelado!\n");
                    sleep(1);
                    run = false;
                    break;
                }
                if (!e) {
                    run = false;
                }
            }
            if (c == 2) {
                e = !e;
            }
            clrscr();
        }
    }
}

// valida que un dia exista
bool validar_dia(int dia, int mes, int anio) {
    if (dia > dias_del_mes(mes, anio)) {
        return false;
    }
    return true;
}

// muestra los turnos de un dia especifico y una cancha especifica 
void print_turnosxdia(int mes, int anio, int cancha) {
    int dia;
    printf("Introduzca el dia del mes que desea ver: ");
    scanf("%d", &dia);
    if (!validar_dia(dia, mes, anio)) {
        printf("Dia invalido!\n");
        while (!validar_dia(dia, mes, anio)) {
            printf("Introduzca el dia del mes que desea ver: ");
            scanf("%d", &dia);
        }
    }
    bool* d = turnos_disponibles(dia, mes, anio, cancha);
    clrscr();
    bool run = true;
    bool e = true;
    while (run) {
        printf("Dia: %d de %s\n", dia, nombre_de_mes(mes));
        printf("Cancha: %d\n\n", cancha);
        char nombre[30];
        for (int i = 0; i < 6; i++) {
            switch (d[i]) {
                case true:
                    printf(SET_COLOR_GREEN "Turno %d: Libre\n", i+1);
                    break;
                case false:
                    strcpy(nombre, find_cliente_by_dni(get_turno(dia, mes, anio, cancha, i+1).cliente_id).nombreyapellido);
                    printf(SET_COLOR_RED "Turno %d: Ocupado por %s\n", i+1, nombre);
                    break;
            }
        }
        printf(SET_COLOR_RESET "\n");
        int c = menu_2_opciones("Ver turno", "Volver", e);
        int id_turno;
        if (c == 1) {
            if (e) {
                printf("Introduzca el turno que desea ver: ");
                while (run) {
                    scanf("%d", &id_turno);
                    if (id_turno >= 1 && id_turno <= 6) {
                        editar_turno(dia, mes, anio, id_turno, cancha, d[id_turno-1]);
                        run = false;
                        break;
                    } else {
                        printf("Turno invalido, intente de nuevo: ");
                    }
                }
            }
            if (!e) {
                run = false;
            }
        }
        if (c == 2) {
            e = !e;
        }
        clrscr();
    }
}

// muestra un mes completo
void print_mes(int mes, int anio, int id_cancha) {
    int firstofthemonth = get_dia_de_semana(anio, mes, 1);
    int dias = dias_del_mes(mes, anio);
    printf("%s %d\n", nombre_de_mes(mes-1), anio);
    printf("  Lun  Mar  Mie  Jue  Vie  Sab  Dom  \n");
    int k;
    int row = 1;
    for (k = 0; k < firstofthemonth; k++) {
        printf("     ");
    }
    for (int j = 1; j <= dias; j++) {
        switch (get_day_color(turnos_disponibles(j, mes, anio, id_cancha))) {
            case 1: // si todos los turnos estan disponibles imprimir el dia en color verde
                printf(SET_COLOR_GREEN "%5d", j);
                break;
            case 2: // si hay turnos ocupados imprime el dia en amarillo
                printf(SET_COLOR_YELLOW "%5d", j);
                break;
            case 3: // si no hay turnos imprimir el dia en color rojo
                printf(SET_COLOR_RED "%5d", j);
                break;
        }
        printf(SET_COLOR_RESET);
        if (++k > 6) {
            k = 0;
            row++;
            printf("\n");
        }
    }
    if (row < 6) {
        printf("\n");
    }
}

// ejecuta y ordena las funciones del calendario
void menu_calendario() {
    int dia, mes, dia_semana, anio, id_cancha = 1;
    int canchas_len = get_cantidad_de_canchas();
    SYSTEMTIME t;
    GetLocalTime(&t);
    dia = t.wDay;
    mes = t.wMonth;
    anio = t.wYear;
    bool run = true;
    cancha c;
    while (run) {
        printf("Hoy es %s %d de %s\n", nombre_de_semana(t.wDayOfWeek-1), t.wDay, nombre_de_mes(t.wMonth - 1));
        print_mes(mes, anio, id_cancha);
        printf("\n");
        c = get_cancha_by_id(id_cancha);
        printf("           <-- %d  %s -->\n", c.id, get_nombre_cancha(c.tipo));
        printf("[UpArrow] Mes anterior   ");
        printf("[DwArrow] Siguiente mes\n");
        printf("[   X   ] Volver al menu principal\n");
        printf("[ Enter ] Ver turnos del dia\n");
        switch (getch()) {
            case UP_KEY:
                if (mes == 0) {
                    mes = 11;
                    anio--;
                } else {
                    mes--;
                }
                break;
            case DOWN_KEY:
                if (mes == 11) {
                    mes = 0;
                    anio++;
                } else {
                    mes++;
                }
                break;
            case LEFT_KEY:
                if (id_cancha > 1) {
                    id_cancha--;
                }
                break;
            case RIGHT_KEY:
                if (id_cancha < canchas_len) {
                    id_cancha++;
                }
                break;
            case ENTER_KEY:
                print_turnosxdia(mes, anio, id_cancha);
                break;
            case X_KEY:
                run = false;
                break;
        }
        clrscr();
    }
}

// almacena y muestra los menús del sistema
void print_menu(int option, int menu_number) {
    if (menu_number == 0) {  // menu principal
            switch (option) {
                case 1: 
                back_color(HOVER_COLOR);
                printf("Menu clientes\n");
                back_color(DEFAULT_COLOR);
                break;
                default:
                printf("Menu clientes\n");
                
            }
            switch (option) {
                case 2:
                back_color(HOVER_COLOR);
                printf("Menu turnos\n");
                back_color(DEFAULT_COLOR);
                break;
                default:
                printf("Menu turnos\n");
            }
            switch (option) {
                case 3:
                back_color(HOVER_COLOR);
                printf("Menu canchas\n");
                back_color(DEFAULT_COLOR);
                break;
                default:
                printf("Menu canchas\n");
            }
            switch (option) {
                case 4:
                back_color(HOVER_COLOR);
                printf("Menu ganancias\n");
                back_color(DEFAULT_COLOR);
                break;
                default:
                printf("Menu ganancias\n");
            }
            switch (option) {
                case 5:
                back_color(HOVER_COLOR);
                printf("Calendario\n");
                back_color(DEFAULT_COLOR);
                break;
                default:
                printf("Calendario\n");
            }
            switch (option) {
                case 6:
                back_color(HOVER_COLOR);
                printf("Salir\n");
                back_color(DEFAULT_COLOR);
                break;
                default:
                printf("Salir\n");
            }
    }
    if (menu_number == 1) {  // menu clientes
        switch (option) {
            case 1:
            back_color(HOVER_COLOR);
            printf("Agregar cliente\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Agregar cliente\n");
        }
        switch (option) {
            case 2:
            back_color(HOVER_COLOR);
            printf("Mostrar clientes\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Mostrar clientes\n");
        }
        switch (option) {
            case 3:
            back_color(HOVER_COLOR);
            printf("---------\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("---------\n");
        }    
        switch (option) {
            case 4:
            back_color(HOVER_COLOR);
            printf("---------\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("---------\n");
        }
        switch (option) {
            case 5:
            back_color(HOVER_COLOR);
            printf("---------\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("---------\n");
        }
            switch (option) {
            case 6:
            back_color(HOVER_COLOR);
            printf("Volver\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Volver\n");
            break;
        }
    }
    if (menu_number == 2) {  // menu turnos
        switch (option) {
            case 1:
            back_color(HOVER_COLOR);
            printf("Agregar turno\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Agregar turno\n");
        }
        switch (option) {
            case 2:
            back_color(HOVER_COLOR);
            printf("Mostrar turnos\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Mostrar turnos\n");
        }
        switch (option) {
            case 3:
            back_color(HOVER_COLOR);
            printf("---------\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("---------\n");
        }    
        switch (option) {
            case 4:
            back_color(HOVER_COLOR);
            printf("---------\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("---------\n");
        }
        switch (option) {
            case 5:
            back_color(HOVER_COLOR);
            printf("---------\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("---------\n");
        }
            switch (option) {
            case 6:
            back_color(HOVER_COLOR);
            printf("Volver\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Volver\n");
            break;
        }
    }
    if (menu_number == 3) {  // menu canchas
        switch (option) {
            case 1:
            back_color(HOVER_COLOR);
            printf("Agregar cancha\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Agregar cancha\n");
        }
        switch (option) {
            case 2:
            back_color(HOVER_COLOR);
            printf("Mostrar canchas\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Mostrar canchas\n");
        }
        switch (option) {
            case 3:
            back_color(HOVER_COLOR);
            printf("Editar precios\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Editar precios\n");
        }    
        switch (option) {
            case 4:
            back_color(HOVER_COLOR);
            printf("---------\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("---------\n");
        }
        switch (option) {
            case 5:
            back_color(HOVER_COLOR);
            printf("---------\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("---------\n");
        }
            switch (option) {
            case 6:
            back_color(HOVER_COLOR);
            printf("Volver\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Volver\n");
            break;
        }
    }  // menu cancha    
    if (menu_number == 4) {  // menu ganancias
        switch (option) {
            case 1:
            back_color(HOVER_COLOR);
            printf("Ver ganancias del dia\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Ver ganancias del dia\n");
        }
        switch (option) {
            case 2:
            back_color(HOVER_COLOR);
            printf("Ver ganancias del mes\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Ver ganancias del mes\n");
        }
        switch (option) {
            case 3:
            back_color(HOVER_COLOR);
            printf("Ver ganancias del anio\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Ver ganancias del anio\n");
        }    
        switch (option) {
            case 4:
            back_color(HOVER_COLOR);
            printf("---------\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("---------\n");
        }
        switch (option) {
            case 5:
            back_color(HOVER_COLOR);
            printf("---------\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("---------\n");
        }
            switch (option) {
            case 6:
            back_color(HOVER_COLOR);
            printf("Volver\n");
            back_color(DEFAULT_COLOR);
            break;
            default:
            printf("Volver\n");
            break;
        }
    }
}

// ejecuta las opciones de los menús
void ejecutador(int menu_number, int function_number) {
    if (menu_number == 1) {  // funciones del menu 1, clientes
        switch (function_number) {
            case 1:
                cargar_cliente();
                break;
            case 2:
                mostrar_clientes();
                volver();
                break;
            default:
                break;
        }
    }
    if (menu_number == 2) {  // funciones del menu 2, turnos
        switch (function_number) {
            case 1:
                cargar_turnos();
                break;
            case 2:
                mostrar_turnos();
                volver();
                break;
            default:
                break;
        }
    }
    if (menu_number == 3) {  // funciones del menu 3, canchas
        switch (function_number) {
            case 1:
                cargar_cancha();
                break;
            case 2:
                mostrar_canchas();
                volver();
                break;
            default:
                break;
        }
    }
    if (menu_number == 4) {  // funciones del menu 4, ganancias
        switch (function_number) {
            case 1:
                show_ganancias(1);
                break;
            case 2:
                show_ganancias(2);
                break;
            case 3:
                show_ganancias(3);
                break;
            default:
                break;
        }
    }
}

// muestra el y le da funcionalidad al menú
void menu(int menu_number) {
    bool run = true;
    int i = 1;
    while (run) {
        print_menu(i, menu_number);
        int c = getch();
        clrscr();
        switch (c) {
        case UP_KEY:
            if (i > 1) {i--;}
            break;
        case DOWN_KEY:
            if (i < 6) {i++;}
            break;
        case 48:
            run = false;
            break;
        case 13:
            switch (i) {
                case 1:
                ejecutador(menu_number, 1);
                break;
                case 2:
                ejecutador(menu_number, 2);
                break;
                case 3:
                ejecutador(menu_number, 3);
                break;
                case 4:
                ejecutador(menu_number, 4);
                break;
                case 5:
                // funcion 5
                break;
                case 6:
                run = false;
                break;
            }
        }
    }
}

// muestra el menu principal
void main_menu() {
    bool run = true;
    int i = 1;
    while (run) {
        print_menu(i, 0);
        int c = getch();
        clrscr();
        switch (c) {
        case UP_KEY:
            if (i > 1) {i--;}
            break;
        case DOWN_KEY:
            if (i < 6) {i++;}
            break;
        case 48:
            run = false;
            break;
        case ENTER_KEY:
            switch (i) {
                case 1:
                menu(1); // abre el menu clientes
                break;
                case 2:
                menu(2); // abre el menu turnos
                break;
                case 3:
                menu(3); // abre el menu canchas
                break;
                case 4:
                menu(4); // abre el menu ganancias
                break;
                case 5:
                menu_calendario();
                break;
                case 6:
                run = false;
                break;
            }
        }
    }
}

int main() {
    hidecursor();
    clrscr();
    main_menu();
    return 0;
}