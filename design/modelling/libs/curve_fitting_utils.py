import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import lmfit


def split_train_test(df: pd.DataFrame, test_size: float = 0.2) -> tuple:
    if (test_size < 0 or test_size > 1):
        raise Exception("test_size should be between 0 and 1.")
    train = df.iloc[:int(len(df) * (1 - test_size))]
    test = df.iloc[int(len(df) * (1 - test_size)):]
    return train, test


def plot_AB(df_A: pd.DataFrame, df_B: pd.DataFrame, y_names: list, label_A: str, label_B: str, suptitle: str, title_prefix: str):
    size = len(y_names)
    fig, ax = plt.subplots(size, 1, figsize=(11, 3 * size), sharex=True)
    fig.suptitle(suptitle, fontsize="x-large")
    for i in range(len(ax)):
        ax[i].set_title(f'{title_prefix} for: {y_names[i]}')
        ax[i].plot(df_A.index, df_A[y_names[i]], label=label_A)
        ax[i].plot(df_B.index, df_B[y_names[i]], label=label_B)
        ax[i].legend(loc='upper right')
    fig.tight_layout()
    plt.show()


def plot_train_test(df_train, df_test, y_names: list):
    plot_AB(df_train, df_test, y_names, "Train data", "Test data",
            "Train/test dataset split visualization", "Train/Test")


def model_fit(model: lmfit.model.Model, params, method, df_train: pd.DataFrame, y_names: list, u_names: list, point_of_operation: dict, df_weights: pd.DataFrame = None, fit_kws: dict = None) -> tuple:
    # Input data
    t = df_train.index.to_numpy(dtype=np.float64)
    U_data = df_train[[*u_names]].to_numpy(dtype=np.float64)

    # True Output data
    Y_data = df_train[[*y_names]].to_numpy(dtype=np.float64).T
    if df_weights is not None:
        W_data = df_weights[[*y_names]].to_numpy(dtype=np.float64).T
    else:
        W_data = None

    # Fit the model
    result = model.fit(
        data=Y_data,
        params=params,
        method=method,
        weights=W_data,
        T=t,
        U=U_data,
        fit_kws=fit_kws,
    )
    return result


def model_eval(result: lmfit.model.ModelResult, df_test: pd.DataFrame, y_names: list, u_names: list, point_of_operation: dict, params=None):
    # Input data
    t = df_test.index.to_numpy(dtype=np.float64)
    U_data = df_test[[*u_names]].to_numpy(dtype=np.float64)

    if params is None:
        yout = result.eval(T=t, U=U_data)
    else:
        yout = result.eval(T=t, U=U_data, params=params)

    # Estimated Output data
    df_out = pd.DataFrame().reindex_like(df_test)
    for i in range(len(y_names)):
        df_out[y_names[i]] = yout[i]

    return df_out


def simulate(result: lmfit.model.ModelResult, df_test: pd.DataFrame, y_names: list, u_names: list, point_of_operation: dict, params=None):
    df_yout = model_eval(result, df_test[[*u_names]], y_names, u_names, point_of_operation, params).iloc[1:]
    for col in u_names:
        df_yout[col] = df_test[col]
    return df_yout


def eval_and_plot(result: lmfit.model.ModelResult, df_train: pd.DataFrame, df_test: pd.DataFrame, y_names: list, u_names: list, point_of_operation: dict, params=None):
    df_train_yin = df_train[[*y_names]]
    df_train_yout = model_eval(
        result, df_train[[*u_names]], y_names, u_names, point_of_operation, params).iloc[1:]
    df_train_yerr = pd.DataFrame(
        (df_train_yin - df_train_yout) / df_train_yin, columns=y_names).iloc[1:]
    df_train_ymse = ((df_train_yin - df_train_yout)
                     ** 2).sum() / df_train_yin.size

    df_test_yin = df_test[[*y_names]]
    df_test_yout = model_eval(
        result, df_test[[*u_names]], y_names, u_names, point_of_operation, params).iloc[1:]
    df_test_yerr = pd.DataFrame(
        (df_test_yin - df_test_yout) / df_test_yin, columns=y_names).iloc[1:]
    df_test_ymse = ((df_test_yin - df_test_yout) ** 2).sum() / df_test_yin.size

    size = len(y_names)
    for col in y_names:
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(11, 3 * size),
                                       sharex=True, gridspec_kw={"height_ratios": [4, 1]})

        fig.suptitle(f"Data vs Model - output: ${col}$", fontsize="x-large")
        # ax1.set_title("Signals")
        ax1.plot(df_train_yin.index, df_train_yin[col], label='Data (train)')
        ax1.plot(df_train_yout.index,
                 df_train_yout[col], label='Model (train)')
        ax1.plot(df_test_yin.index, df_test_yin[col], label='Data (test)')
        ax1.plot(df_test_yout.index, df_test_yout[col], label='Model (test)')
        ax1.legend(loc="lower left")

        ax2.set_title("Errors")
        ax2.plot(df_train_yerr.index,
                 df_train_yerr[col], label="%Error (train)")
        ax2.plot(df_train_yerr.index, np.ones(df_train_yerr.index.size)
                 * df_train_ymse[col], '--', label="MSE (train)")
        ax2.plot(df_test_yerr.index, df_test_yerr[col], label="%Error (test)")
        ax2.plot(df_test_yerr.index, np.ones(df_test_yerr.index.size)
                 * df_test_ymse[col], '--', label="MSE (test)")
        ax2.legend(loc="upper left")

        fig.tight_layout()
        plt.show()
