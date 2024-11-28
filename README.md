# Lightning Piggy — SatsConf 2024

> Um cofrinho Bitcoin Lightning que utiliza LNBits (ou Nostr Wallet Connect), funcionando em um hardware especial com tela ePaper.
> Esta é uma edição especial do projeto [Lightning Piggy](https://lightningpiggy.com/), preparada para a Área Kids da [SatsConf 2024](https://satsconf.com.br/) — toda a interface em português.

[![License: LGPL-2.1](https://img.shields.io/github/license/LightningPiggy/lightning-piggy)](https://github.com/LightningPiggy/lightning-piggy/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/v/release/LightningPiggy/lightning-piggy)](https://github.com/LightningPiggy/lightning-piggy/releases)
[![GitHub stars](https://img.shields.io/github/stars/LightningPiggy/lightning-piggy)](https://github.com/LightningPiggy/lightning-piggy/stargazers)
[![GitHub issues](https://img.shields.io/github/issues/LightningPiggy/lightning-piggy)](https://github.com/LightningPiggy/lightning-piggy/issues)
[![GitHub last commit](https://img.shields.io/github/last-commit/LightningPiggy/lightning-piggy)](https://github.com/LightningPiggy/lightning-piggy/commits/master)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/LightningPiggy/lightning-piggy)

### Requisitos
Para configurar seu cofrinho Lightning, você precisará:

- Um porquinho (Lightning Piggy) [montado](./HARDWARE.md) e [gravado (flashed)](./FLASHING.md) com esta versão de SatsConf 2024
- Uma conta em uma instância do LNBits (para **testes** você pode usar https://lnbits.casa21.space/ - mas lembre-se que LNBits é custodial, então os sats não estão sob seu controle)
- Extensão do LNBits "Pay Links" ativada, com um "Pay Link" criado
- _Opcional_: uma URL NWC (Nostr Wallet Connect) caso prefira receber por NWC em vez de LNBits

No LNBits (extensão "Pay Links"):

- Clique em "New Pay Link"
- Desmarque "fixed amount"
- Valor mínimo `1`, máximo `100000000`, moeda "satoshis"
- Em "Advanced options": "Comment maximum characters" = `128`
- _Opcional_: "Success message" em português, ex.: "Obrigado por enviar sats para meu cofrinho!"

### Como configurar o porquinho

1. Ligue o porquinho: ele cria a rede WiFi `Piggy Config` (modo configuração abre automaticamente quando não há WiFi salvo; para entrar nele depois, mantenha apertado o botão traseiro por 3 segundos).
2. No celular, conecte-se ao WiFi `Piggy Config` (o portal abre sozinho; se não abrir, navegue até http://192.168.4.1)
3. Faça login com o usuário `piggy` e senha `oinkoink`
4. Preencha:
   - Rede WiFi e senha
   - Em "Conexão da carteira": o `Endereço Lightning ou código LNURL-pay` da sua conta (ou a NWC URL), e os dados do servidor LNBits (domínio, porta HTTPS `443` e a `Chave de fatura/leitura`)
5. Toque em `Salvar` e depois `Reiniciar`

O porquinho já vem com padrões brasileiros prontos (idioma Português, fuso America/Sao_Paulo, moeda BRL, modo de suspensão "infinito") aplicados aos campos deixados em branco.

Após conectar, o porquinho mostra o QR para pagamento e cada pix que chegar (sat) aparece na tela.

### Diferenças desta edição (fork SatsConf 2024)

- Versão `6.4.0-sc24` (sufixo marca a edição da SatsConf e evita falsos avisos de atualização em relação ao upstream)
- Todo o conteúdo na tela e no portal de configuração em português
- Padrões pt-BR (Português / America/Sao_Paulo / BRL) aplicados automaticamente
- Modo de suspensão infinita por padrão (acorda pelo botão ou sensor de inclinação)
- Ferramentas de compilação oficiais dentro de Docker (`Containerfile` + `./docker-build.sh`), inclusive gerando o portal e o fuso horário
- NWC: usa o `lightning:` e o Lightning Address do URL NWC como código de recebimento

### Compilando este fork

Não é preciso instalar o Arduino no computador — tudo acontece dentro de Docker:

```
./docker-build.sh
```

Os binários ficam em `Main/build/esp32.esp32.esp32/` (inclui `Main.ino.merged.bin` completo para gravação em um clique). Para uma sessão interativa com a toolchain montada: `./docker-build.sh shell`.

## Agradecimentos

- [LNBits](https://lnbits.com/) por fornecer o serviço de pagamento Lightning
- [Casa21 & VinteUm](https://vinteum.org/) por fornecer a instancia do LNBits para demonstração na [SatsConf 2024](https://satsconf.com.br/), hardware para demonstração com as crianças e aos hackers de lá que ajudaram na reta final da modificação do projeto para a lingua portuguesa
- [Lightning Piggy](https://lightningpiggy.com/) por fornecer o código base desse fork

---

## Para quem é técnico

O firmware compila integralmente dentro de Docker (Arduino CLI 1.1.1 + esp32 core 3.2.0), e o portal de configuração e o fuso horário são gerados automaticamente no build. Detalhes técnicos, arquitetura e screenshots ficam no [AGENTS.md](AGENTS.md). Para os corajosos, há também [emulação com QEMU (incluindo WiFi!)](Emulation.md).
