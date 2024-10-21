#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql/mysql.h>

MYSQL* conectar_bd();
void ejecutar_consulta(char* consulta, char* respuesta);

// Código principal
int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char buff[512];
	char buff2[512];
	
	// INICIALIZACIONES
	// Abrimos el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error creando socket");
		return -1;
	}
	
	// Configuración del socket
	memset(&serv_adr, 0, sizeof(serv_adr)); // Inicializa a cero serv_adr
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); // Escucharemos en cualquier IP de la máquina
	serv_adr.sin_port = htons(9050); // Puerto 9050
	
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0) {
		printf("Error al hacer bind");
		return -1;
	}
	
	// La cola de peticiones pendientes no podrá ser superior a 2
	if (listen(sock_listen, 2) < 0) {
		printf("Error en el Listen");
		return -1;
	}
	
	// Bucle para aceptar conexiones continuamente
	while (1) {
		printf("Esperando conexiones...\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		if (sock_conn < 0) {
			printf("Error al aceptar conexión");
			continue;
		}
		printf("Conexión recibida\n");
		
		// Recibimos datos del cliente
		ret = read(sock_conn, buff, sizeof(buff));
		printf("Recibido\n");
		
		// Añadir el terminador nulo al final de buff
		buff[ret] = '\0';
		printf("Mensaje recibido: %s\n", buff);
		
		// Analizamos el mensaje recibido
		char *p = strtok(buff, "/");
		int codigo = atoi(p);
		p = strtok(NULL, "/");
		char id_usuario[50];
		strcpy(id_usuario, p);
		printf("Codigo: %d, ID Usuario: %s\n", codigo, id_usuario);
		
		// Limpiar respuesta
		memset(buff2, 0, sizeof(buff2));
		
		// Definir la consulta según el código recibido
		char consulta[256];
		if (codigo == 1) {
			// Consultar nombre del usuario
			sprintf(consulta, "SELECT Nombre_Usuario FROM usuario WHERE ID_Usuario = ?");
		} else if (codigo == 2) {
			// Consultar correo del usuario
			sprintf(consulta, "SELECT mail FROM usuario WHERE ID_Usuario = ?");
		} else if (codigo == 3) {
			// Consultar contraseña del usuario
			sprintf(consulta, "SELECT Password FROM usuario WHERE ID_Usuario = ?");
		} else {
			strcpy(buff2, "Código no válido");
			write(sock_conn, buff2, strlen(buff2));
			close(sock_conn);
			continue;
		}
		
		// Ejecutar la consulta y obtener la respuesta
		ejecutar_consulta(consulta, id_usuario, buff2);
		
		printf("Respuesta: %s\n", buff2);
		// Enviamos la respuesta al cliente
		write(sock_conn, buff2, strlen(buff2));
		
		// Se acabó el servicio para este cliente
		close(sock_conn);
	}
	
	// Cerrar el socket del servidor
	close(sock_listen);
	
	return 0;
}

// Función para conectarse a la base de datos
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

// Función para ejecutar consultas y obtener resultados
void ejecutar_consulta(char* consulta, char* id_usuario, char* respuesta) {
	MYSQL *conn = conectar_bd();
	MYSQL_STMT *stmt;
	MYSQL_BIND bind[1];
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	stmt = mysql_stmt_init(conn);
	if (stmt == NULL) {
		printf("Error inicializando el statement: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error inicializando el statement");
		mysql_close(conn);
		return;
	}
	
	if (mysql_stmt_prepare(stmt, consulta, strlen(consulta)) != 0) {
		printf("Error preparando la consulta: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error preparando la consulta");
		mysql_stmt_close(stmt);
		mysql_close(conn);
		return;
	}
	
	// Bind de parámetros para consultas preparadas
	memset(bind, 0, sizeof(bind));
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = (char *)id_usuario;
	bind[0].buffer_length = strlen(id_usuario);
	
	if (mysql_stmt_bind_param(stmt, bind) != 0) {
		printf("Error en el bind de parámetros: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en el bind de parámetros");
		mysql_stmt_close(stmt);
		mysql_close(conn);
		return;
	}
	
	// Ejecutar consulta
	if (mysql_stmt_execute(stmt) != 0) {
		printf("Error ejecutando la consulta: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error ejecutando la consulta");
		mysql_stmt_close(stmt);
		mysql_close(conn);
		return;
	}
	
	// Obtener resultados
	res = mysql_stmt_result_metadata(stmt);
	if (res == NULL) {
		printf("Error obteniendo los resultados: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_stmt_close(stmt);
		mysql_close(conn);
		return;
	}
	
	if ((row = mysql_fetch_row(res))) {
		sprintf(respuesta, "%s", row[0]);
	} else {
		strcpy(respuesta, "Usuario no encontrado");
	}
	
	// Liberar recursos
	mysql_free_result(res);
	mysql_stmt_close(stmt);
	mysql_close(conn);
}
