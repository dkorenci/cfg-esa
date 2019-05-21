# run grammar compression all files in a folder, output relevan statistics
# params: folder

cmd=$1
folder=$2

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


echo "fname string_size sum_of_tree_depths alpha"

for fname in $folder/*; do
	$1 -f $fname -s > out.txt
	stats=`cat stats.txt`
	pyramid_size=$(get_arg_value "$stats" "sum_of_tree_depths")
	str_size=$(get_arg_value "$stats" "string_size")
	alpha=$(get_arg_value "$stats" "alpha")
	echo $fname $str_size $pyramid_size $alpha
done

