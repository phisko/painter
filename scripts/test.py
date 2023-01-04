def callback(delta_time):
    print("Test from Python")

comp = kengine.self.get_python()

behavior = kengine.self.emplace_execute()
behavior.func = callback

for i in range(0, comp.scripts.size()):
    s = comp.scripts.get(i)
    print(s)
