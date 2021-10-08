cd /app/build
cmake /app/
make -j
mv /app/build/*.so /app/tutorial/
cd /app/tutorial
python3 t.py

