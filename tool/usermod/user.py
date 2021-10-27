# Class representing a user, which is one row in the .db
    # has a name and one or many ids.

class User():
    
    def __init__(self, **kwargs):
        # "Default constructor"
        self.firstname = "DEFAULT"
        self.lastname = "DEFAULT"
        self.ids = ["FFFFFFFF","FFFFFFFF","FFFFFFFF","FFFFFFFF","FFFFFFFF","FFFFFFFF"]
        
        # "Custom constructor"
        if (kwargs.get("firstname")):
            self.firstname = kwargs.get("firstname")
        if (kwargs.get("lastname")):
            self.lastname = kwargs.get("lastname")
        if (kwargs.get("id")):
            self.ids[0] = kwargs.get("id")
       
                
    def print_user(self):
        print("Printing user...")
        print("\t Name: {} {}".format(self.firstname, self.lastname))
        print("\t IDs:")
        for i in self.ids:
            if (i != "FFFFFFFF"):
                print(" {}".format(i))
        print("")
