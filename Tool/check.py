import numpy as np
import argparse

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('--input', default='01', help='input file num')
  args = parser.parse_args()

  FILE_IN = f'/home/pp22/share/hw1/testcases/{args.input}.out'
  FILE_OUT = 'out'

  fin = np.fromfile(FILE_IN, dtype=np.float32)
  fout = np.fromfile(FILE_OUT, dtype=np.float32)

  fw = open('log.txt', 'w') 
  print(f'fin: {fin.size}, fout: {fout.size}')
  fw.write(f'fin: {fin.size}, fout: {fout.size}\n')
  if(fin.size != fout.size):
    print('[ERROR] file size different')

  err = 0
  for i in range(fin.size):
    if fin[i] != fout[i]:
      err += 1
      print(f'[ERROR] on data[{i}], expected: {fin[i]}, got: {fout[i]}')
      fw.write(f'[ERROR] on data[{i}], expected: {fin[i]}, got: {fout[i]}\n')
  print(f'error rate: {err/fin.size}')