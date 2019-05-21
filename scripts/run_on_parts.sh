# run grammar compression on 1/N, 2/N, ... N/N of the file
# params: command for grammar compression, file

cmd=$1
fname=$2
N=$3

# extracts value from $1 labeled by $2, 
# $1 constist of pairs of the form 'label: value'
function get_arg_value {   
	return=0
#	echo "arg1: "$1
#	echo "arg2: "$2
	for s in $1;
	do
#	    echo "s: "$s
	    if [ $return -eq 1 ]; then
		echo $s
		break
	    fi	
	    if [ $s == $2: ]; then 	
		return=1
	    fi	
	done
} 

# pobriši sve part* datoteke
# izračunaj N-tine i pomoću heada kreiraj fajlove
result=`wc -c $fname` # get wc output
result=($result) # turn to array
size=${result[0]} # get size
let partSize=size/N
#echo $size $partSize

# zavrti skriptu na part* fajlovima 
# kopiraj fajl sa standardnim outputom i druge fajlove u kopiju 
# označenu određenim djelom
# print output header
echo "part time int_tree_size alpha num_rules string_size compressed_size"
for i in $(seq 1 $N); do 
	# calculate i*1/N*size
	if [[ $i -lt $N ]]
	then
		let fileSize=partSize*i
	else
		fileSize=$size
	fi
	partFile=$fname.part$i # name of part file
	# output i*1/N bytes of fname to partFile
	`head -c $fileSize $fname > $partFile` 
	# run command
	`$1 -f $partFile -s > out.txt`
	stats=`cat stats.txt`
	# output some stats to stdout
	time=$(get_arg_value "$stats" "compression_time")
	tree_size=$(get_arg_value "$stats" "interval_tree_size")
	nnt=$(get_arg_value "$stats" "num_non_terminals")
	nt=$(get_arg_value "$stats" "num_terminals")		
	let comprSize=nnt+nt
	str_size=$(get_arg_value "$stats" "string_size")		
	num_rules=$(get_arg_value "$stats" "num_rules")		
	alpha=$(get_arg_value "$stats" "alpha")
	echo $i $time $tree_size $alpha $num_rules $str_size $comprSize
	# store stats
	`mv stats.txt stats.$i.txt` 
done


