import os
import pandas as pd
import matplotlib.pyplot as plt

experiment_dir = "exp/exp_9/"
cases = os.listdir(experiment_dir)

plt.figure(figsize=(10, 5))
result_df = []

cases = [case for case in cases if os.path.isdir(os.path.join(experiment_dir, case))]

for i, case in enumerate(cases):
    result_dirs = os.listdir(os.path.join(experiment_dir, case, "use_db"))
    df_temp = None
    for rd in result_dirs:
        rd_path = os.path.join(experiment_dir, case, "use_db", rd, "convergence.csv")
        df = pd.read_csv(rd_path, header=None)
        df = df.rename(columns={df.columns[0]: 'Frame', df.columns[1]: 'Count'})

        if df_temp is None:
            df_temp = df
        else:
            df_temp = pd.concat([df_temp, df])
            df_temp = df_temp.reset_index(drop=True)

    result_df.append(df_temp)
    print(f"--- {case} ---")
    df_not_converged = df_temp[df_temp['Count'] == -1]
    print(f"Number of non-converged cases: {len(df_not_converged)}")
    df_filtered = df_temp[df_temp['Count'] != -1]
    print(f"Mean: {df_filtered['Count'].mean()}")
    print(f"Median: {df_filtered['Count'].median()}")
    print(f"Std: {df_filtered['Count'].std()}")
    plt.hist(df_filtered['Count'], bins=100, alpha=0.5, label=case)
    result_df[i].to_csv(os.path.join(experiment_dir, f"{case}_convergence.csv"), index=False)

plt.xlabel('Count')
plt.ylabel('Frequency')
plt.title('Convergence')
plt.legend()
plt.savefig(os.path.join(experiment_dir, "convergence.png"))
plt.show()
