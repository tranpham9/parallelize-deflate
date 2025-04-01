echo "2.5 MB\n"
for i in $(seq 1 5); do ./build/main testFiles/randomValues_0-255/2.5MB | grep Total; done

echo "3.0 MB\n"
for i in $(seq 1 5); do ./build/main testFiles/randomValues_0-255/3.0MB | grep Total; done

echo "3.5 MB\n"
for i in $(seq 1 5); do ./build/main testFiles/randomValues_0-255/3.5MB | grep Total; done

echo "4.0 MB\n"
for i in $(seq 1 5); do ./build/main testFiles/randomValues_0-255/4.0MB | grep Total; done

echo "4.5 MB\n"
for i in $(seq 1 5); do ./build/main testFiles/randomValues_0-255/4.5MB | grep Total; done

echo "5.0 MB\n"
for i in $(seq 1 5); do ./build/main testFiles/randomValues_0-255/5.0MB | grep Total; done
