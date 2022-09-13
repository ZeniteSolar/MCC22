import mpl_toolkits.mplot3d.axes3d as p3
from matplotlib import cm
from scipy.interpolate import griddata
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

dataset_filename = "simulations_ltspice/simulation_optimization_dataset.csv"
df = pd.read_csv(dataset_filename, low_memory=False)
df = df.apply(pd.to_numeric, errors='coerce')
df = df.replace(np.nan, 0)
df_backup = df

df = df_backup[['fsw', 'Li', 'Cc', 'Lo', 'V(eff)']]
df = df.groupby(['fsw', 'Li', 'Cc', 'Lo']).mean().reset_index()
df


def get_xyz(df: pd.DataFrame, X: str, Y: str, Z: str):
    X = df[X].to_numpy()
    Y = df[Y].to_numpy()
    Z = df[Z].to_numpy()

    # create x-y points to be used in heatmap
    xi = np.linspace(X.min(), X.max(), 1000)
    yi = np.linspace(Y.min(), Y.max(), 1000)

    # Interpolate for plotting
    zi = griddata((X, Y), Z, (xi[None, :], yi[:, None]), method='cubic')

    # Create the contour plot
    # CS = plt.contourf(xi, yi, zi, 15, cmap=plt.cm.rainbow,
    #                   vmax=zmax, vmin=zmin)
    # plt.colorbar()
    # plt.show()

    xi, yi = np.meshgrid(xi, yi)
    return xi, yi, zi


fig = plt.figure(figsize=(10, 10))
ax = plt.axes(projection='3d')
ax.contour3D(*get_xyz(df.loc[df['fsw'] == 40000.0],
             'Li', 'Cc', 'V(eff)'), 50, cmap='Blues')
ax.contour3D(*get_xyz(df.loc[df['fsw'] == 100000.0],
             'Li', 'Cc', 'V(eff)'), 50, cmap='Reds')
ax.set_xlabel('Li [H]')
ax.set_ylabel('Cc [F]')
ax.set_zlabel('eff')
# ax.view_init(30, 290)
# ax.set_title('3D contour for cosine')
plt.show()


# fig = plt.figure(figsize=(10, 10))
# ax = plt.axes(projection='3d')
# ax.contour3D(*get_xyz(df.loc[df['fsw'] == 100000.0],
#              'Li', 'Cc', 'V(eff)'), 50, cmap='Blues')
# ax.contour3D(*get_xyz(df.loc[df['fsw'] == 100000.0],
#              'Lo', 'Cc', 'V(eff)'), 50, cmap='Reds')
# ax.set_xlabel('[H]')
# ax.set_ylabel('Cc [F]')
# ax.set_zlabel('eff')
# ax.view_init(30, 290)
# # ax.set_title('3D contour for cosine')
# plt.show()
