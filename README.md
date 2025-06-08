# EE

%%{init: {'theme': 'base', 'themeVariables': { 'primaryColor': '#ffffff', 'primaryTextColor': '#000', 'lineColor': '#333', 'secondaryColor': '#f0f0f0', 'tertiaryColor': '#fff'}}}%%
graph TD
    subgraph "Processo Principal (Máquina de Estados)"
        direction TB

        %% Definição dos Estados com Variáveis Internas %%
        S_INIT(((<b>INICIALIZANDO</b><br>Motor: MOVENDO<br>Dir: 0 (Descer)<br>PWM: 40 a 50%<br>Timer_2s: INATIVO)))
        S_REPOUSO(((<b>REPOUSO</b><br>Motor: PARADO<br>Dir: N/A<br>PWM: 0%<br>Timer_2s: INATIVO)))
        S_SUBINDO(((<b>SUBINDO</b><br>Motor: MOVENDO<br>Dir: 1 (Subir)<br>PWM: 40 a 50%<br>Timer_2s: INATIVO)))
        S_DESCENDO(((<b>DESCENDO</b><br>Motor: MOVENDO<br>Dir: 0 (Descer)<br>PWM: 40 a 50%<br>Timer_2s: INATIVO)))
        S_PARADO(((<b>PARADO NO ANDAR</b><br>Motor: PARADO<br>Dir: N/A<br>PWM: 0%<br>Timer_2s: ATIVO)))

        %% Transições de Estado %%
        S_INIT -- "Sensor S1 (andar 0) ativado" --> S_REPOUSO
        S_REPOUSO -- "Fila de percurso não está vazia" --> S_SUBINDO
        
        S_SUBINDO -- "Andar de destino alcançado<br>OU Sensor S4 (proteção) ativado" --> S_PARADO
        S_SUBINDO -- "Fim das solicitações de subida<br>& existem solicitações abaixo" --> S_DESCENDO
        
        S_DESCENDO -- "Andar de destino alcançado" --> S_PARADO
        S_DESCENDO -- "Sensor S1 (andar 0) ativado" --> S_REPOUSO
        
        S_PARADO -- "Timer 2s expirado<br>& há mais solicitações acima" --> S_SUBINDO
        S_PARADO -- "Timer 2s expirado<br>& há mais solicitações abaixo" --> S_DESCENDO
        S_PARADO -- "Timer 2s expirado<br>& sem mais solicitações" --> S_DESCENDO
    end

    subgraph "Processo de Interrupção (Paralelo)"
        ISR[("<b>ISR: Tratamento da Interrupção UART</b><br>1. Lê o dado recebido do Bluetooth<br>2. Valida o comando (ex: $OD)<br>3. Adiciona solicitação à fila de percurso<br>4. Limpa flag de interrupção")]
    end

    %% Seta de Interrupção %%
    subgraph " "
        edge[color:white, style:invis]
        edge -.-> ISR
    end
    linkStyle 10 stroke:red,stroke-width:2px,stroke-dasharray: 5 5;
    
    subgraph " "
        direction LR
        S_REPOUSO -.- edge
        S_SUBINDO -.- edge
        S_DESCENDO -.- edge
        S_PARADO -.- edge
        S_INIT -.- edge
    end
    linkStyle 0,1,2,3,4,5,6,7,8,9 stroke-width:2px
    linkStyle 11,12,13,14,15 stroke:white, stroke-width:0px

    %% Estilização %%
    style S_INIT fill:#cce5ff,stroke:#004085,stroke-width:2px
    style S_REPOUSO fill:#d4edda,stroke:#155724,stroke-width:2px
    style S_SUBINDO fill:#f8d7da,stroke:#721c24,stroke-width:2px
    style S_DESCENDO fill:#f8d7da,stroke:#721c24,stroke-width:2px
    style S_PARADO fill:#fff3cd,stroke:#856404,stroke-width:2px
    style ISR fill:#e2e3e5,stroke:#383d41,stroke-width:2px
