compile:
	gcc -lpthread -lrt producer.c -o producer
	gcc -lpthread -lrt consumer.c -o consumer
	gcc -lpthread -lrt controller.c -o controller

run: run_producer run_consumer1 run_consumer2 run_controller

run_producer:
	./producer

run_consumer1:
	sleep 5
	./consumer

run_consumer2:
	sleep 10
	./consumer

run_controller:
	./controller
