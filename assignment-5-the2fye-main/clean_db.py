import sqlite3

conn = sqlite3.connect("database.db")
c = conn.cursor()

c.execute("DELETE FROM notes")
conn.commit()

print("Rows deleted:", c.rowcount)

conn.close()