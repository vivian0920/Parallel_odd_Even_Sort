import numpy as np
import argparse

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('--input', default='01', help='input file num')
  args = parser.parse_args()

  FILE_NAME = f'/home/pp22/share/hw1/testcases/{args.input}.in'

  data = np.fromfile(FILE_NAME, dtype=np.float32)
  print(f'num: {data.size}')

  for d in data:
    print(d)