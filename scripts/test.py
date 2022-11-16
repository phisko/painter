def callback(deltaTime):
    print("Test from Python")

comp = kengine.self.getPythonComponent()

behavior = kengine.self.attachExecute()
behavior.func = callback

for i in range(0, comp.scripts.size()):
    s = comp.scripts.get(i)
    print(s)
