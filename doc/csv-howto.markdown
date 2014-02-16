CSV howto
=========

CSV is a simple format, which can be used to store and read tabular data. The
cxxtools serialization framework supports reading and writing CSV files. The CSV
serializer makes it easy and robust to create CSV files from serializable
objects and the CSV deserializer reads deserializable objects from a CSV file.

Although CSV is quite simple it has its traps and pitfalls like Data, which
contain the used delimiter or line feeds. When CSV files are written straight
forward, frequently those details will be wrong. Cxxtools helps here.
