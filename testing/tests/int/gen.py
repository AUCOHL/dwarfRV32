

for i in range(16):
    print("""
.ifndef IRQ%d
.weak IRQ%d
IRQ%d:
		uret
.endif
""" % (i,i, i))

