# Please type script directory in first arguments

# ${n} means that n-th input arguments ps. ${0} is script name
echo -e "${0}"


for f in ${1}/atpg*.script ;
do 
echo -e "./bin/opt/fan -f ${f}";
./bin/opt/fan -f ${f} ;
done

for f in ${1}/fsim*.script ;
do 
echo -e "./bin/opt/fan -f ${f}";
./bin/opt/fan -f ${f} ;
done