#!/bin/bash

LOG_FILE="system_monitor.log"
ALERT_THRESHOLD_CPU=80
ALERT_THRESHOLD_MEM=80
ALERT_THRESHOLD_DISK=90
SLEEP_INTERVAL=60

# Cria o arquivo de log se não existir e inicializa com uma mensagem
if [ ! -f "$LOG_FILE" ]; then
    touch "$LOG_FILE"
    # Adiciona uma mensagem inicial ao log.
    echo "$(date +'%Y-%m-%d %H:%M:%S') - Arquivo de log criado" >> $LOG_FILE
fi

# Escreve uma mensagem no arquivo de log com timestamp.
function log_message {
    local message="$1"
    # Uso de 'tee -a' para adicionar mensagens ao log e também para que elas sejam exibidas no console.
    echo "$(date +'%Y-%m-%d %H:%M:%S') - $message" | tee -a $LOG_FILE
}

# Envia uma mensagem de alerta.
function send_alert {
    local message="$1"
    # Uso de 'tee /dev/tty' para exibir as mensagens de alerta no console e registrar no log.
    echo "$message" | tee /dev/tty # Exibe a mensagem no console e grava no log
}

while true; do
    #  Calcula o uso de CPU utilizando o comando top
    CPU_USAGE=$(top -bn1 | grep "Cpu(s)" | \
        sed "s/.*, *\([0-9.]*\)%* id.*/\1/" | \
        awk '{print 100 - $1}')
    
    # Calcula o uso de memória utilizando o comando free
    MEMORY_USAGE=$(free | grep Mem | \
        awk '{print $3/$2 * 100.0}')
    
    # Verifica o uso de espaço em disco nas partições principais
    DISK_USAGE_ROOT=$(df / | grep / | awk '{print $5}' | sed 's/%//')
    DISK_USAGE_HOME=$(df /home | grep /home | awk '{print $5}' | sed 's/%//')

    # Verifica se o uso de CPU excede 80%. Se sim, registra um alerta no log e envia uma mensagem de alerta
    if (( $(echo "$CPU_USAGE > $ALERT_THRESHOLD_CPU" | bc -l) )); then
        log_message "ALERTA: Uso de CPU alto - $CPU_USAGE%"
        send_alert "ALERTA: Uso de CPU alto - $CPU_USAGE%"
    fi

    # Verifica se o uso de memória excede 80%. Se sim, registra um alerta no log e envia uma mensagem de alerta
    if (( $(echo "$MEMORY_USAGE > $ALERT_THRESHOLD_MEM" | bc -l) )); then
        log_message "ALERTA: Uso de memória alto - $MEMORY_USAGE%"
        send_alert "ALERTA: Uso de memória alto - $MEMORY_USAGE%"
    fi

    # Verifica se o uso do espaço em disco na partição root (/) excede o limiar de 90%
    if (( $DISK_USAGE_ROOT > $ALERT_THRESHOLD_DISK )); then
        log_message "ALERTA: Uso de espaço em disco na partição / alto - $DISK_USAGE_ROOT%"
        send_alert "ALERTA: Uso de espaço em disco na partição / alto - $DISK_USAGE_ROOT%"
    fi

    # Verifica se o uso do espaço em disco na partição /home excede o limiar de 90%
    if (( $DISK_USAGE_HOME > $ALERT_THRESHOLD_DISK )); then
        log_message "ALERTA: Uso de espaço em disco na partição /home alto - $DISK_USAGE_HOME%"
        send_alert "ALERTA: Uso de espaço em disco na partição /home alto - $DISK_USAGE_HOME%"
    fi

    # O script roda indefinidamente, verificando o uso de CPU e memória a cada minuto (SLEEP_INTERVAL=60).
    sleep $SLEEP_INTERVAL
done
