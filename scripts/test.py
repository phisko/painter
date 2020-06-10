def callback(deltaTime):
    print("PYTE")

comp = pk.self.getPythonComponent()

behavior = pk.self.attachExecute()
behavior.func = callback

for i in range(0, comp.scripts.size()):
    s = comp.scripts.get(i)
    print(s)
