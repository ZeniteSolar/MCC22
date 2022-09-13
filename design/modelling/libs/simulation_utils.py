import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def plot_simulation(df_in, df_out):
    for col in df_out.columns:
        plt.figure(figsize=(20, 5))
        plt.title(col)
        plt.plot(df_in.index, df_in[col], label='PSIM Simulation data')
        plt.plot(df_out.index, df_out[col], label='State Space Model Simulation data')
        plt.legend(loc='lower right')
        plt.xlim(df_out.index[0], df_out.index[-1])
        plt.show()


def downsampling_array(arr:np.array, n: int) -> np.array:
    end = n * int(len(arr) / n)
    return np.mean(arr[:end].reshape(-1, n), 1)


def downsampling_dataframe(df: pd.DataFrame, k: int) -> pd.DataFrame:
    records = {}
    df = df.reset_index(drop=False)
    for col in df.columns:
        records[col] = downsampling_array(df[col].to_numpy(dtype=df[col].dtype), k)
    return pd.DataFrame.from_records(records, index='t')
