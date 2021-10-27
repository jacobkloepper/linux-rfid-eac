# Class representing a user, which is one line in the .csv
    # has a name and one or many ids.

import sqlite3
import os

class data_handler():
    
    def __init__(self, **kwargs):
        # "Default constructor"
        self.path = "Y:\\_to\\jacob\\test\\data\\user_data\\users.db"
        
        # "Custom constructor"
        if (kwargs.get("path")):
            self.path = kwargs.get("path")
    
    
        # Setup connection
        self.conn = sqlite3.connect(self.path)
        self.c = self.conn.cursor()
        
        # Set up table
        
        #self.c.execute("""CREATE TABLE users (
        #                 firstname text,
        #                 lastname text,
        #                 id0 text,
        #                 id1 text,
        #                 id2 text,
        #                 id3 text,
        #                 id4 text
        #                 )""")
        #self.conn.commit()
        #self.conn.close()
       
    def print_table(self):
    # Just print the table out.
        with self.conn:
            self.c.execute("SELECT * FROM users")
            table = self.c.fetchall()
            for row in table:
                print(row)
    
    def in_user(self, user, id):
    # Check if user has id, return True/False
        with self.conn:
            self.c.execute("SELECT id0, id1, id2, id3, id4 FROM users WHERE (firstname=? AND lastname=?)", (user.firstname, user.lastname))
            table = self.c.fetchall()

        for row in table:
            for item in row:
                if (item == id):
                    return True
        return False

    def in_table(self, user):
    # Check if table has user, return True/False
        with self.conn:
            self.c.execute("SELECT * FROM users WHERE (firstname=? AND lastname=?)", (user.firstname, user.lastname))
            table = self.c.fetchall()
        
        if not table:
            return False
        else:
            return True
    
    def clear_table(self):
    # Clear the table (not delete)
        with self.conn:
            self.c.execute("DELETE FROM users")
        
    def add_user(self, user):
    # Adds user to table, fails if user is already in table.
        if (not self.in_table(user)):
            with self.conn:
                self.c.execute("INSERT INTO users VALUES (?, ?, ?, ?, ?, ?, ?)", (user.firstname, user.lastname, user.ids[0], user.ids[1], user.ids[2], user.ids[3], user.ids[4]))
                
            print("SUCCESS: [{} {}] added.".format(user.firstname,user.lastname))
        else:
            print("FAILURE: [{} {}] already in table.".format(user.firstname,user.lastname))
        
    def rm_user(self, user):
    # Removes user from table, fails if user is not in table.
        if (self.in_table(user)):
            sql = "DELETE FROM users WHERE  (firstname=? AND lastname=?)"
            with self.conn:
                self.c.execute(sql, (user.firstname, user.lastname))
            
            
            print("SUCCESS: [{} {}] deleted.".format(user.firstname,user.lastname))
        else:
            print("FAILURE: [{} {}] not in table.".format(user.firstname,user.lastname))
        
    def edit_user(self, user, cmd, id):
    # Check if user is in table
        if (self.in_table(user)):
            # Get row data for this user
            self.c.execute("SELECT * FROM users WHERE (firstname=? AND lastname=?)", (user.firstname, user.lastname))
            row = self.c.fetchone()
            sql = "UPDATE users SET firstname=?, lastname=?, id0=?, id1=?, id2=?, id3=?, id4=? WHERE (firstname=? AND lastname=?)"
            new_row = []
            
            # Turn id blank if it is in row
            if (cmd == "rm"):
                for item in row:
                    if (item == id):
                        new_row.append("FFFFFFFF")
                    else:
                        new_row.append(item)
    
            
            # Overwrite first blank id in row
            elif (cmd == "add"):
                if (self.in_user(user, id)):
                    print("FAILURE: [{} {}] already has ID [{}].".format(user.firstname, user.lastname, id))
                    return
                else:
                    added = False
                    for item in row:
                        if (item == "FFFFFFFF" and added is False):
                            new_row.append(id)
                            added = True
                        else:
                            new_row.append(item)
                    
                    # If added is still false here, there were no blank ids in the table.
                    if (added == False):
                        print("FAILURE: [{} {}] cannot have any more IDs. Consider deleting old IDs.".format(user.firstname, user.lastname))
                        return
                    
            else:
                print("Improper edit command: {}".format(cmd))
                    
            # Execute
            new_row.append(user.firstname)
            new_row.append(user.lastname)
            with self.conn:
                self.c.execute(sql, tuple(new_row))
            
            print("SUCCESS: [{} {}] edited".format(user.firstname, user.lastname))
        else:
            print("FAILURE: [{} {}] not in table".format(user.firstname, user.lastname))
