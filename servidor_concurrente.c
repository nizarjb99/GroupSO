#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <mysql/mysql.h>

#define MAX_CLIENTS 100

pthread_mutex_t mutex_lista_jugadores;
char jugadores_conectados[MAX_CLIENTS][50];
int num_jugadores = 0;

MYSQL* conectar_bd();
void* atender_cliente(void* socket_desc);
void agregar_jugador(const char* jugador);
void quitar_jugador(const char* jugador);
void enviar_lista_jugadores(int sock_conn);

int main(int argc, char* argv[]) {
	int sock_listen, sock_conn;
	struct sockaddr_in serv_adr;
	pthread_t thread_id;
	
	// Inicializar mutex
	pthread_mutex_init(&mutex_lista_jugadores, NULL);
	
	// Crear socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error creando socket");
		exit(1);
	}
	
	// Configurar la dirección del servidor
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(9050);
	
	// Hacer bind al puerto
	if (bind(sock_listen, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) < 0) {
		perror("Error en el bind");
		exit(1);
	}
	
	// Escuchar conexiones
	if (listen(sock_listen, MAX_CLIENTS) < 0) {
		perror("Error en el listen");
		exit(1);
	}
	
	printf("Servidor escuchando en el puerto 9050...\n");
	
	// Aceptar conexiones de clientes
	while (1) {
		sock_conn = accept(sock_listen, NULL, NULL);
		if (sock_conn < 0) {
			perror("Error en el accept");
			continue;
		}
		
		printf("Nueva conexión aceptada\n");
		
		// Crear un hilo para atender al cliente
		int* new_sock = malloc(1);
		*new_sock = sock_conn;
		if (pthread_create(&thread_id, NULL, atender_cliente, (void*)new_sock) < 0) {
			perror("No se pudo crear el hilo");
			close(sock_conn);
			free(new_sock);
		} else {
			pthread_detach(thread_id); // Liberar recursos del hilo cuando termine
		}
	}
	
	// Cerrar el socket de escucha
	close(sock_listen);
	pthread_mutex_destroy(&mutex_lista_jugadores);
	return 0;
}

void* atender_cliente(void* socket_desc) {
	int sock_conn = *(int*)socket_desc;
	free(socket_desc);
	char buffer[512];
	int ret;
	
	// Recibir el nombre del jugador
	ret = read(sock_conn, buffer, sizeof(buffer));
	if (ret <= 0) {
		close(sock_conn);
		return NULL;
	}
	buffer[ret] = '\0';
	
	printf("Jugador conectado: %s\n", buffer);
	agregar_jugador(buffer);
	
	while (1) {
		// Recibir solicitud del cliente
		ret = read(sock_conn, buffer, sizeof(buffer));
		if (ret <= 0) {
			printf("Cliente desconectado: %s\n", buffer);
			quitar_jugador(buffer);
			close(sock_conn);
			return NULL;
		}
		buffer[ret] = '\0';
		
		// Procesar solicitud
		if (strcmp(buffer, "LISTA_JUGADORES") == 0) {
			enviar_lista_jugadores(sock_conn);
		} else {
			// Procesar otras solicitudes (consultar base de datos, etc.)
			write(sock_conn, "Solicitud no reconocida", strlen("Solicitud no reconocida"));
		}
	}
	
	return NULL;
}

void agregar_jugador(const char* jugador) {
	pthread_mutex_lock(&mutex_lista_jugadores);
	if (num_jugadores < MAX_CLIENTS) {
		strcpy(jugadores_conectados[num_jugadores], jugador);
		num_jugadores++;
	}
	pthread_mutex_unlock(&mutex_lista_jugadores);
}

void quitar_jugador(const char* jugador) {
	pthread_mutex_lock(&mutex_lista_jugadores);
	for (int i = 0; i < num_jugadores; i++) {
		if (strcmp(jugadores_conectados[i], jugador) == 0) {
			// Mover el último jugador a la posición del jugador que se desconectó
			strcpy(jugadores_conectados[i], jugadores_conectados[num_jugadores - 1]);
			num_jugadores--;
			break;
		}
	}
	pthread_mutex_unlock(&mutex_lista_jugadores);
}

void enviar_lista_jugadores(int sock_conn) {
	pthread_mutex_lock(&mutex_lista_jugadores);
	char buffer[1024] = "Jugadores conectados:\n";
	for (int i = 0; i < num_jugadores; i++) {
		strcat(buffer, jugadores_conectados[i]);
		strcat(buffer, "\n");
	}
	pthread_mutex_unlock(&mutex_lista_jugadores);
	
	write(sock_conn, buffer, strlen(buffer));
}

// Conexion a la base de datos
MYSQL* conectar_bd() {
	MYSQL *conn;
	conn = mysql_init(NULL);
	if (conn == NULL) {
		printf("Error al crear el objeto MySQL: %s\n", mysql_error(conn));
		exit(1);
	}
	
	// Conectar a la base de datos
	if (mysql_real_connect(conn, "localhost", "usuario", "password", "BBDD_PARCHIS", 0, NULL, 0) == NULL) {
		printf("Error al conectar a la base de datos: %s\n", mysql_error(conn));
		mysql_close(conn);
		exit(1);
	}
	
	return conn;
}

// Consultar nombre del usuario
void consultar_nombre(char* id_usuario, char* respuesta) {
	MYSQL *conn = conectar_bd();
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	char consulta[256];
	sprintf(consulta, "SELECT Nombre_Usuario FROM usuario WHERE ID_Usuario = '%s'", id_usuario);
	
	if (mysql_query(conn, consulta)) {
		printf("Error en la consulta: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	res = mysql_store_result(conn);
	if (res == NULL) {
		printf("Error al obtener resultados: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	
	if ((row = mysql_fetch_row(res))) {
		sprintf(respuesta, "Nombre: %s", row[0]);
	} else {
		strcpy(respuesta, "Usuario no encontrado");
	}
	
	mysql_free_result(res);
	mysql_close(conn);
}

// Consultar correo del usuario
void consultar_mail(char* id_usuario, char* respuesta) {
	MYSQL *conn = conectar_bd();
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	char consulta[256];
	sprintf(consulta, "SELECT mail FROM usuario WHERE ID_Usuario = '%s'", id_usuario);
	
	if (mysql_query(conn, consulta)) {
		printf("Error en la consulta: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	
	res = mysql_store_result(conn);
	if (res == NULL) {
		printf("Error al obtener resultados: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	
	if ((row = mysql_fetch_row(res))) {
		sprintf(respuesta, "Correo: %s", row[0]);
	} else {
		strcpy(respuesta, "Usuario no encontrado");
	}
	
	mysql_free_result(res);
	mysql_close(conn);
}

// Consultar contraseÃ±a del usuario
void consultar_password(char* id_usuario, char* respuesta) {
	MYSQL *conn = conectar_bd();
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	char consulta[256];
	sprintf(consulta, "SELECT Password FROM usuario WHERE ID_Usuario = '%s'", id_usuario);
	
	if (mysql_query(conn, consulta)) {
		printf("Error en la consulta: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	
	res = mysql_store_result(conn);
	if (res == NULL) {
		printf("Error al obtener resultados: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	
	if ((row = mysql_fetch_row(res))) {
		sprintf(respuesta, "Contraseña: %s", row[0]);
	} else {
		strcpy(respuesta, "Usuario no encontrado");
	}
	
	mysql_free_result(res);
	mysql_close(conn);
}




