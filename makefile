server:qq_server.o qq_json.o qq_sql.o 
	gcc -o server  qq_server.o qq_json.o qq_sql.o -lsqlite3 -ljson -lpthread
%.o:%.c
	gcc -c $^ -o $@
client:qq_client.o qq_json.o qq_sql.o 
	gcc -o client  $^ -lsqlite3 -ljson -lpthread
%.o:%.c
	gcc -c $^ -o $@

clean:
	rm -rf .*.sw? *.o
