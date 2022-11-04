import json
import seaborn as sns
import numpy as np
import pandas as pd
from scipy.stats import norm
import matplotlib.pyplot as plt

from typing import Dict, Tuple, Any, List


def setup_style():
    sns.set_style("whitegrid")
    CB91_Blue = '#2CBDFE'
    CB91_Green = '#47DBCD'
    CB91_Pink = '#F3A0F2'
    CB91_Purple = '#9D2EC5'
    CB91_Violet = '#661D98'
    CB91_Amber = '#F5B14C'
    color_list = [CB91_Blue, CB91_Pink, CB91_Green, CB91_Amber, CB91_Purple, CB91_Violet]

    plt.rcParams['axes.prop_cycle'] = plt.cycler(color=color_list)
    plt.rcParams['figure.dpi'] = 400

    sns.set(font='Franklin Gothic Book', rc={
        'axes.grid': True,
        'axes.axisbelow': True,
        'axes.edgecolor': 'lightgrey',
        'axes.facecolor': 'None',
        'axes.labelcolor': 'dimgrey',
        'axes.spines.right': False,
        'axes.spines.top': False,
        'grid.color': 'lightgrey',
        'grid.alpha': 0.5,
        'figure.facecolor': 'white',
        'lines.solid_capstyle': 'round',
        'patch.edgecolor': 'w',
        'patch.force_edgecolor': True,
        'text.color': 'dimgrey',
        'xtick.bottom': False,
        'xtick.color': 'dimgrey',
        'xtick.direction': 'out',
        'xtick.top': False,
        'ytick.color': 'dimgrey',
        'ytick.direction': 'out',
        'ytick.left': False,
        'ytick.right': False}
    )

    # sns.set_context("notebook", rc={"font.size": 16,
    #                                 "axes.titlesize": 20,
    #                                 "axes.labelsize": 18})


def load_results(results_dir: str) -> Tuple[Dict[str, Any], List[Any]]:
    jemalloc_results = json.load(open(f'{results_dir}/jemalloc.json', 'r'))['benchmarks']
    mimalloc_results = json.load(open(f'{results_dir}/mimalloc.json', 'r'))['benchmarks']
    mpp_results = json.load(open(f'{results_dir}/mpp.json', 'r'))['benchmarks']
    ptmalloc2_results = json.load(open(f'{results_dir}/ptmalloc2.json', 'r'))['benchmarks']
    ptmalloc3_results = json.load(open(f'{results_dir}/ptmalloc3.json', 'r'))['benchmarks']
    rpmalloc_results = json.load(open(f'{results_dir}/rpmalloc.json', 'r'))['benchmarks']

    mpp_results_mem_access = json.load(open('mpp_access_memory.json', 'r'))['benchmarks']

    return ({
        'jemalloc': jemalloc_results,
        'mimalloc': mimalloc_results,
        'mpp': mpp_results,
        'ptmalloc2': ptmalloc2_results,
        'ptmalloc3': ptmalloc3_results,
        'rpmalloc': rpmalloc_results,
    }, mpp_results_mem_access)


def filter_name(bm_name: str, bm_to_find: str) -> bool:
    return bm_name.startswith(bm_to_find) and not bm_name.endswith('_cv') and \
        not bm_name.endswith('_stddev') and \
        not bm_name.endswith('_median') and \
        not bm_name.endswith('_mean')


def main():
    setup_style()

    results_all, results_mpp_mem_acc = load_results('2022-10-26T-16_26_25Z')

    complex_pick = [
        "BM_Complex/\"Total ops: \" \"200'000\" \"Transition matrix: ver-1\"/iterations:5",
        "BM_Complex/\"Total ops: \" \"1'000'000\" \"Transition matrix: ver-1\"/iterations:5",
        "BM_Complex/\"Total ops: \" \"2'000'000\" \"Transition matrix: ver-1\"/iterations:5",
        "BM_AllocateManyRandom/4096",
        "BM_DeallocateManyRandom/4096"
    ]

    results_complex_200k = []
    results_complex_1000k = []
    results_complex_2000k = []
    results_allocate_4096 = []
    results_deallocate_4096 = []

    for allocator, results in results_all.items():
        if allocator == 'ptmalloc3': # or allocator == 'jemalloc':
            continue
        bm_name = complex_pick[0]
        bm_mean_time = [
            bm['real_time'] for bm in results if filter_name(bm['name'], bm_name)
        ]
        bm_mean_memory = [
            bm['PeakMemoryUsage'] for bm in results if filter_name(bm['name'], bm_name)
        ]
        results_complex_200k.append((allocator, bm_name, np.array(bm_mean_time), np.array(bm_mean_memory)))

        bm_name = complex_pick[1]
        bm_mean_time = [
            bm['real_time'] for bm in results if filter_name(bm['name'], bm_name)
        ]
        bm_mean_memory = [
            bm['PeakMemoryUsage'] for bm in results if filter_name(bm['name'], bm_name)
        ]
        results_complex_1000k.append((allocator, bm_name, np.array(bm_mean_time), np.array(bm_mean_memory)))

        bm_name = complex_pick[2]
        bm_mean_time = [
            bm['real_time'] for bm in results if filter_name(bm['name'], bm_name)
        ]
        bm_mean_memory = [
            bm['PeakMemoryUsage'] for bm in results if filter_name(bm['name'], bm_name)
        ]
        results_complex_2000k.append((allocator, bm_name, np.array(bm_mean_time), np.array(bm_mean_memory)))

        bm_name = complex_pick[3]
        bm_mean_time = [
            bm['real_time'] for bm in results if filter_name(bm['name'], bm_name)
        ]
        results_allocate_4096.append((allocator, bm_name, np.array(bm_mean_time)))

        bm_name = complex_pick[4]
        bm_mean_time = [
            bm['real_time'] for bm in results if filter_name(bm['name'], bm_name)
        ]
        results_deallocate_4096.append((allocator, bm_name, np.array(bm_mean_time)))

    # fig, ax = plt.subplots()
    # for allocator, bm_name, bm_mean, bm_stddev in results_200k:
    #     x = np.linspace(bm_mean - 3 * bm_stddev, bm_mean + 3 * bm_stddev, 1000)
    #     y = norm.pdf(x, bm_mean, bm_stddev)
    #     ax.plot(x, y, label=allocator)
    #     ax.fill_between(x, 0, y, alpha=.3)

    plt.figure()
    df = pd.DataFrame(results_allocate_4096, columns=['allocator', 'name', 'time'])
    df = df.explode('time')
    ax = sns.boxplot(x="allocator", y="time", data=df)
    ax.set_title('Benchmark Allocate 4096 (time)')
    ax.axhline(df[df['allocator'] == 'mpp']['time'].mean(), 0, 1, color='r', linestyle='--')
    plt.plot()
    plt.savefig('allocate-4096-time.png')

    plt.figure()
    df = pd.DataFrame(results_deallocate_4096, columns=['allocator', 'name', 'time'])
    df = df.explode('time')
    ax = sns.boxplot(x="allocator", y="time", data=df)
    ax.set_title('Benchmark Deallocate 4096 (time)')
    ax.axhline(df[df['allocator'] == 'mpp']['time'].mean(), 0, 1, color='r', linestyle='--')
    plt.plot()
    plt.savefig('deallocate-4096-time.png')

    # Plot results 200k on the same graph (boxplot)
    plt.figure()
    df = pd.DataFrame(results_complex_200k, columns=['allocator', 'name', 'mean', 'memory'])
    df = df.explode('mean')
    df = df.explode('memory')
    ax = sns.boxplot(x="allocator", y="mean", data=df)
    ax.set_title('Complex benchmark 200k (time)')
    ax.axhline(df[df['allocator'] == 'mpp']['mean'].mean(), 0, 1, color='r', linestyle='--')
    plt.plot()
    plt.savefig('complex_200k-time.png')

    plt.figure()
    ax = sns.barplot(x="allocator", y="memory", data=df)
    ax.set_title('Complex benchmark 200k (mem)')
    ax.axhline(df[df['allocator'] == 'mpp']['memory'].mean(), 0, 1, color='r', linestyle='--')
    plt.plot()
    plt.savefig('complex_200k-mem.png')

    # Plot results 1000k on the same graph (boxplot)
    plt.figure()
    # remove ptmalloc3 from results_1000k
    df = pd.DataFrame(results_complex_1000k, columns=['allocator', 'name', 'mean', 'memory'])
    df = df.explode('mean')
    df = df.explode('memory')
    ax = sns.boxplot(x="allocator", y="mean", data=df)
    ax.set_title('Complex benchmark 1m (time)')
    ax.axhline(df[df['allocator'] == 'mpp']['mean'].mean(), 0, 1, color='r', linestyle='--')
    plt.plot()
    plt.savefig('complex_1m-time.png')

    plt.figure()
    ax = sns.barplot(x="allocator", y="memory", data=df)
    ax.set_title('Complex benchmark 1m (mem)')
    ax.axhline(df[df['allocator'] == 'mpp']['memory'].mean(), 0, 1, color='r', linestyle='--')
    plt.plot()
    plt.savefig('complex_1m-mem.png')

    # Plot results 2000k on the same graph (boxplot)
    plt.figure()
    df = pd.DataFrame(results_complex_2000k, columns=['allocator', 'name', 'mean', 'memory'])
    df = df.explode('mean')
    df = df.explode('memory')
    ax = sns.boxplot(x="allocator", y="mean", data=df)
    ax.set_title('Complex benchmark 2m (time)')
    ax.axhline(df[df['allocator'] == 'mpp']['mean'].mean(), 0, 1, color='r', linestyle='--')
    plt.plot()
    plt.savefig('complex_2m-time.png')

    plt.figure()
    ax = sns.barplot(x="allocator", y="memory", data=df)
    ax.set_title('Complex benchmark 2m (mem)')
    ax.axhline(df[df['allocator'] == 'mpp']['memory'].mean(), 0, 1, color='r', linestyle='--')
    plt.plot()
    plt.savefig('complex_2m-mem.png')

    benchmark_types = {
        'BM_AccessMemoryDefaultLinkedList': 'Default',              # Normal linked list without layouting
        'BM_AccessMemoryRandomizedLayoutedLinkedList': 'Layouted',  # Randomized linked list with layouting
        'BM_AccessMemoryRandomizedLinkedList': 'Randomized'         # Randomized linked list
    }

    memory_pick = [
        "/2048/manual_time",
        "/8192/manual_time",
        "/16384/manual_time"
    ]

    results_2k = []
    results_8k = []
    results_16k = []

    for bm_type, bm_simple_name in benchmark_types.items():
        bm_name = bm_type + memory_pick[0]
        bm_mean_time = [
            bm['real_time'] for bm in results_mpp_mem_acc if filter_name(bm['name'], bm_name)
        ]
        results_2k.append((bm_simple_name, np.array(bm_mean_time)))

        bm_name = bm_type + memory_pick[1]
        bm_mean_time = [
            bm['real_time'] for bm in results_mpp_mem_acc if filter_name(bm['name'], bm_name)
        ]
        results_8k.append((bm_simple_name, np.array(bm_mean_time)))

        bm_name = bm_type + memory_pick[2]
        bm_mean_time = [
            bm['real_time'] for bm in results_mpp_mem_acc if filter_name(bm['name'], bm_name)
        ]
        results_16k.append((bm_simple_name, np.array(bm_mean_time)))

    plt.figure()
    df = pd.DataFrame(results_2k, columns=['name', 'time'])
    df = df.explode('time')
    ax = sns.barplot(x="name", y="time", data=df)
    ax.set_ylabel('Time (microseconds)')
    ax.set_title('Memory access benchmark. List size = 2048')
    plt.plot()
    plt.savefig('mem_access-2048.png')

    plt.figure()
    df = pd.DataFrame(results_8k, columns=['name', 'time'])
    df = df.explode('time')
    ax = sns.barplot(x="name", y="time", data=df)
    ax.set_ylabel('Time (microseconds)')
    ax.set_title('Memory access benchmark. List size = 8192')
    plt.plot()
    plt.savefig('mem_access-8192.png')

    plt.figure()
    df = pd.DataFrame(results_16k, columns=['name', 'time'])
    df = df.explode('time')
    ax = sns.barplot(x="name", y="time", data=df)
    ax.set_ylabel('Time (microseconds)')
    ax.set_title('Memory access benchmark. List size = 16384')
    plt.plot()
    plt.savefig('mem_access-16384.png')
    # plt.show()


if __name__ == '__main__':
    main()
