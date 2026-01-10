# mkconf/utils.mk 工具函数

# 颜色
C_GREEN   = $(shell echo "\033[0;32m")
C_YELLOW  = $(shell echo "\033[1;33m")
C_RED     = $(shell echo "\033[0;31m")
C_BLUE    = $(shell echo "\033[0;34m")
C_CYAN    = $(shell echo "\033[0;36m")
C_RESET   = $(shell echo "\033[0m")

# 日志
log_info = @echo "$(C_CYAN)[INFO]$(C_RESET) $1"
log_ok   = @echo "$(C_GREEN)[ OK ]$(C_RESET) $1"
log_warn = @echo "$(C_YELLOW)[WARN]$(C_RESET) $1"
log_err  = @echo "$(C_RED)[ERROR]$(C_RESET) $1"
