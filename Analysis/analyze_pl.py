import polars as pl
import matplotlib.pyplot as plt

parquet_path = "../Data/logs_output.parquet"
lf = pl.scan_parquet(parquet_path)
level_counts_lf = lf.group_by("level").agg(
    pl.len().alias("level_count")
)
action_counts_lf = lf.group_by("action").agg(
    pl.len().alias("action_count")
)
stats_lf = lf.select(
    pl.col("latency_ms").mean().alias("latency_ms_mean"),
    pl.col("latency_ms").median().alias("latency_ms_median"),
    pl.col("latency_ms").max().alias("latency_ms_max"),
    pl.col("latency_ms").min().alias("latency_ms_min"),
    pl.col("latency_ms").quantile(0.95).alias("latency_ms_quantile")
)
print(level_counts_lf.collect())
print(action_counts_lf.collect())
print(stats_lf.collect())