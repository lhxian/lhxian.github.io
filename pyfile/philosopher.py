import tkinter as tk
import math
CNT=5
ANGLE =2* math.pi/CNT
W,H,R=800,600,200
CX ,CY= W/2,H/2
FREE_COLOR="#aaaaaa"
person,stick = [],[]
has_eat = [0]*CNT
stick_takeup = [0]*CNT
colors=["red","green","blue","yellow","#ff00ff"]
root = tk.Tk()
root.geometry(f"{W}x{H}")
cur_angle = 0
for i in range(5):
  p = tk.Label(master=root,text=f"philosopher_{i}",bg=colors[i])
  x,y = CX+R*math.cos(cur_angle), CY+R*math.sin(cur_angle)
  p.place(x=x,y=y,anchor="center")
  person.append(p)
  tem_angle = cur_angle + ANGLE/2
  x,y = CX + R* math.cos(tem_angle),CY + R* math.sin(tem_angle)
  p = tk.Label(master=root,text=f"chopsticks_{i}",bg=FREE_COLOR)
  p.place(x=x,y=y,anchor="center")
  stick.append(p)
  cur_angle += ANGLE

len = len(person)
# counter = 0
def update_once():
  # global counter
  for i in range(len):
    if has_eat[i] == 0: #没有拿到一个筷子
      if i % 2 == 0 and stick_takeup[i] == 0: #先尝试拿一个筷子
        stick_takeup[i] = 1
        stick[i].config(bg=colors[i])
        has_eat[i] = 1
      elif i %2 == 1 and stick_takeup[i-1] == 0:
        stick_takeup[i-1] = 1
        stick[i-1].config(bg = colors[i])
        has_eat[i] = 1


    elif has_eat[i] == 1: #拿到了一个筷子，尝试再拿一个筷子
      if i %2 == 0 and stick_takeup[i-1] == 0:
        stick_takeup[i-1] = 1
        stick[i-1].config(bg=colors[i])
        has_eat[i] = 2
      elif i %2 == 1 and stick_takeup[i] == 0:
        stick_takeup[i] = 1
        stick[i].config(bg=colors[i])
        has_eat[i] = 2
    
    elif has_eat[i] == 2: #拿到了两个筷子，释放两个筷子，将状态重置
      assert stick_takeup[i] and stick_takeup[i-1]
      stick_takeup[i],stick_takeup[i-1] = 0,0
      stick[i].config(bg=FREE_COLOR)
      stick[i-1].config(bg=FREE_COLOR)
      has_eat[i] = 0
  # counter = (counter + 1) %len
  root.after(1000,update_once)

update_once()
root.mainloop()
