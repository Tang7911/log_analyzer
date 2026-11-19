import polars as pl
import plotly.express as px
import plotly.graph_objects as go
import plotly_print

parquet_path = "../Data/logs_output.parquet"
lf = pl.scan_parquet(parquet_path)
level_counts_lf = lf.group_by("level").agg(
    pl.len().alias("level_count")
)
action_counts_lf = lf.group_by("action").agg(
    pl.len().alias("action_count")
)
latency_ms_lf = lf.select(
    pl.col("latency_ms").mean().alias("latency_ms_mean"),
    pl.col("latency_ms").median().alias("latency_ms_median"),
    pl.col("latency_ms").max().alias("latency_ms_max"),
    pl.col("latency_ms").min().alias("latency_ms_min"),
    pl.col("latency_ms").quantile(0.95).alias("latency_ms_quantile")
)
'''
print(level_counts_lf.collect())
print(action_counts_lf.collect())
print(latency_ms_lf.collect())
'''
action_ms = lf.group_by("action").agg(
    pl.col("latency_ms").mean().alias("action_ms_maen"),
    pl.col("latency_ms").max().alias("action_ms_max")
)
error_ratio = lf.select(
    ((pl.col("level") == "ERROR").sum() / pl.len()).alias("error_ratio")
)
slow_ratio = lf.select(
    ((pl.col("latency_ms") > 800).sum() / pl.len()).alias("slow_ratio")
)
result = pl.collect_all([level_counts_lf, action_counts_lf, 
                         latency_ms_lf, action_ms, error_ratio, slow_ratio])

(
    level_df, action_df, latency_ms_df, 
    action_ms_df, error_ratio_df, slow_ratio_df
) = result
print(level_df)
print(action_df)
print(latency_ms_df)
print(action_ms_df)
print(error_ratio_df)
print(slow_ratio_df)

df1 = level_counts_lf.collect()
fig1 = px.bar(
    df1,
    x="level",
    y="level_count",
    color="level",
    title="Log Level Distribution",
    color_discrete_sequence=px.colors.qualitative.Pastel
)
plotly_print.write_image(fig1, "../Data/level_bar.png",scale=2)

df2 = action_counts_lf.collect()
fig2 = px.bar(
    df2,
    x="action",
    y="action_count",
    color="action",
    title="Log Action Distribution",
    color_discrete_sequence=px.colors.qualitative.Pastel
)
plotly_print.write_image(fig2, "../Data/action_bar_pl.png", scale=2)

df3 = lf.select("latency_ms").collect()
fig3 = px.histogram(
    df3,
    x="latency_ms",
    nbins=30
)
plotly_print.write_image(fig3, "../Data/latency_ms_hist_pl.png", scale=2)

error_lf = lf.filter(pl.col("level") == "ERROR")
minute_counts = (
    error_lf
    .group_by(pl.col("timestamp").dt.truncate("1m").alias("time"))
    .len()
    .collect()
)
fig4 = px.line(
    minute_counts,
    x="time",
    y="len",
    title="Errors per minute"
)
fig4.update_layout(
    xaxis_title="time",
    yaxis_title="error_counts",
    hovermode="x unified",
    template="plotly_white"
)
fig4.update_xaxes(
    tickformat="%H:%M",
    dtick=30 * 60 * 1000,
    tickangle=-45
)
fig4.update_yaxes(
    rangemode="tozero"
)
plotly_print.write_image(fig4, "../Data/timestamp_error_pl.png", scale=2)

agg = (
    lf.with_columns((pl.col("level") == "ERROR").alias("is_error"))
      .group_by_dynamic("timestamp", every="10m")
      .agg(pl.col("is_error").sum().alias("error_count"))
)
bounds = lf.select(
    pl.col("timestamp").min().dt.truncate("10m").alias("start"),
    pl.col("timestamp").max().alias("end")
).collect()
grid = pl.DataFrame({
    "timestamp": pl.datetime_range(
        bounds["start"][0], bounds["end"][0], "10m", eager=True
    )
})
df_result = (
    grid.lazy()
        .join(agg, on="timestamp", how="left")
        .with_columns(pl.col("error_count").fill_null(0))
        .sort("timestamp", descending=False)
        .collect(engine="streaming")
)
print(df_result.head())

window_size = 3
df_result = (
    df_result.with_columns(
        pl.col("error_count").shift(1)
          .rolling_mean(window_size=window_size, min_samples=1).alias("roll_mean"),
        pl.col("error_count").shift(1)
          .rolling_std(window_size=window_size, min_samples=1).alias("roll_std")
    ).with_columns(
        (pl.col("roll_mean") + 4 * pl.col("roll_std")).alias("threshold")
    )
)
print(df_result.head())
anomalies = (
    df_result.filter(
        pl.col("threshold").is_not_null() & (pl.col("error_count") > pl.col("threshold"))
    ).select("timestamp", "error_count", "roll_mean", "roll_std", "threshold")
)
print(f"发现{anomalies.height}个异常时间点！！！")
print(anomalies)

fig5 = go.Figure()
fig5.add_trace(go.Scatter(x=df_result["timestamp"], y=df_result["error_count"], name="Error_count"))
fig5.add_trace(go.Scatter(x=df_result["timestamp"], y=df_result["threshold"], name="threshold"))
fig5.update_layout(
    title="Error Rate Anomaly Detection (Rolling Window)",
    xaxis_title="Time",
    yaxis_title="Error Count per 1min"
)
plotly_print.write_image(fig5, "../Data/anomaly_detection_pl.png", scale=2)