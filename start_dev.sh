#!/bin/bash

SESSION="TuixOS_dev"
TUIXOS_PATH="$(pwd)"

# 检查 TuixOS 目录是否存在
if [ ! -d "$TUIXOS_PATH" ]; then
    echo "错误: TuixOS 目录不存在: $TUIXOS_PATH"
    exit 1
fi

# 检查会话是否存在
if tmux has-session -t "$SESSION" 2>/dev/null; then
    echo "Tmux 会话 $SESSION 已存在，正在附加..."
    tmux attach-session -t "$SESSION"
    exit 0
fi

# 创建新会话
echo "创建新的 tmux 会话: $SESSION"

# 创建主会话窗口
tmux new-session -s "$SESSION" -n coding -c "$TUIXOS_PATH" -d

# 设置编码窗口
tmux send-keys -t "$SESSION":coding "cd $TUIXOS_PATH && nvim ." Enter

# 创建右侧运行窗格(左80 右20 注意-p指的是新划分的窗口)
tmux split-window -h -p 20 -t "$SESSION":coding -c "$TUIXOS_PATH" -d
tmux send-keys -t "$SESSION":coding.1 'echo "[Run] 请运行: make clean && make run"' Enter
tmux select-pane -t "$SESSION":coding.0

# 创建构建调试窗口
tmux new-window -t "$SESSION" -n build_debug -c "$TUIXOS_PATH" -d
tmux send-keys -t "$SESSION":build_debug 'echo "[Build] 执行构建..." && make clean && make debug' Enter

# 创建调试窗格(左20 右80)
tmux split-window -h -p 80 -t "$SESSION":build_debug -c "$TUIXOS_PATH" -d
tmux send-keys -t "$SESSION":build_debug.1 'echo "[Debug] 请运行: gdb -x debug/debug-grub.gdb"' Enter
tmux select-pane -t "$SESSION":build_debug.1

# 回到 coding 窗口
tmux select-window -t "$SESSION":coding

# 附加到会话
echo "会话创建完成！正在附加..."
tmux attach-session -t "$SESSION"
