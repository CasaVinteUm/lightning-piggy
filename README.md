# Lightning Piggy
> Um cofrinho Bitcoin Lightning que utiliza LNBits, funcionando em um hardware especial com tela ePaper.

Essa é uma versão comemorativa do projeto Lightning Piggy, que foi criado especialmente para a Área Kids da [SatsConf 2024](https://satsconf.com.br/). Para mais informações sobre o projeto original, visite https://lightningpiggy.com/

### Requisitos e configuração do LNBits
Para configurar seu cofrinho Lightning, você precisará:

- Uma instancia do LNBits (para **testes** você pode usar https://lnbits.casa21.space/ - mas lembre-se que LNBits é custodial, então os sats aqui não estão sob seu controle)
- Uma conta na sua instancia do LNBits
- Extensão do LNBits "Pay Links"
- Um porquinho (Lightning Piggy) [montado](./HARDWARE.md) e [gravado (flashed)](./FLASHING.md)

Na página da sua conta no LNBits:

- Ative a extensão "Pay Links" na seção "Extensions"
- Clique na extensão "Pay Links"
- Clique em "New Pay Link"
- Desmarcar "fixed amount"
- Definir valor mínimo: `1`
- Definir valor máximo: `100000000`
- Definir moeda para "satoshis"
- Clicar em "Advanced options"
- Definir "Comment maximum characters" para `128`
- _Opcional_: Definir "Webhook URL" para https://p.lightningpiggy.com/ (para métricas de uso anônimas)
- _Opcional_: Definir uma "Success message", como por exemplo: "Obrigado por enviar sats para meu cofrinho!"

Após isso, você deverá começar a configurar os parâmetros do seu cofrinho Lightning.

### Como configurar seu Lightning Piggy

- Ao ligar pela primeira vez, o Lightning Piggy irá criar uma rede WiFi com o nome `LN Piggy`
- Conecte-se à rede WiFi `LN Piggy` e navegue até o endereço IP do seu cofrinho Lightning (geralmente, http://192.168.4.1)
- Clique em "Configure WiFi"
- Procure o nome da sua rede WiFi
- Digite a senha da sua rede WiFi
- Defina o paramêtro "LNBits Host" com o endereço http(s) do seu LNBits
- Defina o paramêtro "LNBits Port" com o número da porta do seu LNBits (443 para https, 80 para http)
- Defina o paramêtro "Invoice Key" com a chave de acesso do seu LNBits (encontrada na página principal da sua carteira no LNBits, na seção "API Keys")
- Salve as configurações

Após isso, o Lightning Piggy deverá se conectar automaticamente à sua rede WiFi e mostrar o QRCODE para pagamento.
Pagamentos vão aparecendo na tela do porquinho de tempos em tempos.

## Agradecimentos

- [LNBits](https://lnbits.com/) por fornecer o serviço de pagamento Lightning
- [Casa21 & VinteUm](https://vinteum.org/) por fornecer a instancia do LNBits para demonstração na [SatsConf 2024](https://satsconf.com.br/), hardware para demonstração com as crianças e aos hackers de lá que ajudaram na reta final da modificação do projeto para a lingua portuguesa
- [Lightning Piggy](https://lightningpiggy.com/) por fornecer o código base desse fork

Veja mais em https://www.lightningpiggy.com/ e https://makers.bolt.fun/project/lightningpiggy
Para instalar o software mais recente no seu Lightning Piggy, visite: https://lightningpiggy.github.io
