import math
import numpy as np
import matplotlib.pyplot as plt


sizes = [1000, 10000, 100000, 1000000, 100000000, 1000000000, 10000000000]
indices = np.arange(len(sizes))
ccn_sizes = []
ip_sizes = []
delta_sizes = []
for s in sizes:
    ccn = ccn_size(s)
    ip = ip_size(s)
    ccn_sizes.append(ccn)
    ip_sizes.append(ip)
    delta = (float(ccn) - float(ip)) / float(ip)
    delta_sizes.append(delta)
    print s, ccn, ip


def format_e(n):
    a = '%E' % n
    return a.split('E')[0].rstrip('0').rstrip('.') + 'E' + a.split('E')[1]

width = 0.4 

fig, ax = plt.subplots()
rects = ax.bar(indices, delta_sizes, width * 2, color = 'r')
#rects1 = ax.bar(indices, ccn_sizes, width, color='r', log=True)
#rects2 = ax.bar(indices + width, ip_sizes, width, color='b', log=True)


# add some text for labels, title and axes ticks
ax.set_ylabel('Relative Increase in Data Packets')
ax.set_xlabel('Total Data Size [B]')
ax.set_xticks(indices + (width * 2))
#ax.set_yscale('log')
ax.set_xticklabels(tuple(map(lambda s : "{:.1e}".format(s), sizes)))

#ax.legend((rects1[0], rects2[0]), ('SCR', 'IPBC'), loc=2)
#ax.legend((rects[0]), 

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width()/2., 1.05*height,
                '%d' % int(height),
                ha='center', va='bottom')
#autolabel(rects1)
#autolabel(rects2)
plt.grid(True)
plt.savefig('sizes_relative.eps')


fig, ax = plt.subplots()
rects1 = ax.bar(indices, ccn_sizes, width, color='r', log=True)
rects2 = ax.bar(indices + width, ip_sizes, width, color='b', log=True)

# add some text for labels, title and axes ticks
ax.set_ylabel('Number of Data Packets')
ax.set_xlabel('Total Data Size [B]')
ax.set_xticks(indices + width)
#ax.set_yscale('log')
ax.set_xticklabels(tuple(map(lambda s : "{:.1e}".format(s), sizes)))

ax.legend((rects1[0], rects2[0]), ('SCR', 'IPBC'), loc=2)

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width()/2., 1.05*height,
                '%d' % int(height),
                ha='center', va='bottom')
#autolabel(rects1)
#autolabel(rects2)
plt.grid(True)
plt.savefig('sizes.eps')

