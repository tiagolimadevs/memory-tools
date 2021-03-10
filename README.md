#memory tool
this is a program to scan and set memory in a program

app_name # app name
app_pid # app pid
file_in # shold be a file with range name line
	# stack
	# alloc

file_out #file out
save_seed #store seed to use on "filein" on get, put, monitor

format #format var int,float,string

type # type mode scan,get,monitor,put

example use: 

	./main app $app_name filein $file_in
		fileout $file_out
		format $format
		saveseed $save_seed
		type $type

	./main help #show use mode

