while true
do
	ps -aux|grep TestEnQ_loop | grep -v grep
	sleep 1
done
