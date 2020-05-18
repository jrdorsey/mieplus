from matplotlib import pyplot as plt
import numpy as np

print(plt.style.available)
plt.style.use('seaborn')
dFile = 'output/polytest.csv'
data = np.genfromtxt(dFile, dtype=float, delimiter=',', names=True)
fig, ax = plt.subplots()
ax.plot(data['Cdia'],data['CQs'],'C0-',label='CF')
ax.plot(data['Cdia'],data['CSQs'],'C1-',label='CS')
ax.plot(data['Mdia'],data['MQs'],'C2-',label='MF')
ax.plot(data['Mdia'],data['MSQs'],'C3-',label='MS')
plt.legend()
plt.title('PCASP bin edges')
ax.set_xlabel('Water equivalent diameter (um)')
ax.set_ylabel('PSL equivalent diameter (um)')
plt.show()

Cdia, CQs, CSQs, Mdia, MQs, MSQs
