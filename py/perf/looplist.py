import dis
from pydantic import BaseModel
from typing import List, Optional

mylist1 = []
mylist2 = []
mylist11 = []
mylist22 = []

class MyObj(BaseModel):

    mlst: Optional[List] = []
    mystr: Optional[str]
    myint: Optional[int] = 0

obj1 = MyObj(
    mlst=[1,2,2,3,1,1,2,5,1,21,21,1,1,1,1,13],
    mystr='blahblah',
    myint=99999
)
obj2 = MyObj(
    mlst=[1,2,2,3,1,1,2,5,1,21,21,1,1,1,1,13],
    mystr='blahblah',
    myint=99999
)
obj3 = MyObj(
    mlst=[1,2,2,3,1,1,2,5,1,21,21,1,1,1,1,13],
    mystr='blahblah',
    myint=99999
)
mylist1 = [obj1, obj2, obj3]
mylist2 = [obj1, obj2, obj3]

def forloop(l1):
    for x in l1:
        if x.myint == 99999:
            mylist11.append(x)
    
def list_comp(l2):
    mylist22 = [x for x in l2 if x.myint == 99999]

def main():
    obj1 = MyObj(
        mlst=[1,2,2,3,1,1,2,5,1,21,21,1,1,1,1,13],
        mystr='blahblah',
        myint=99999
    )
    obj2 = MyObj(
        mlst=[1,2,2,3,1,1,2,5,1,21,21,1,1,1,1,13],
        mystr='blahblah',
        myint=99999
    )
    obj3 = MyObj(
        mlst=[1,2,2,3,1,1,2,5,1,21,21,1,1,1,1,13],
        mystr='blahblah',
        myint=99999
    )
    mylist1 = [obj1, obj2, obj3]
    mylist2 = [obj1, obj2, obj3]

    dis.dis(forloop)
    print('--------------------------------------------------')
    dis.dis(list_comp)

if __name__ == '__main__':
    import cProfile
    cProfile.run('forloop(mylist1)')
    cProfile.run('list_comp(mylist2)')
