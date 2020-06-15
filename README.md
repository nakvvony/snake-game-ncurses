# SnakeGame_ncurses
<img width="699" alt="스크린샷 2020-06-16 오전 12 34 12" src="https://user-images.githubusercontent.com/2377324/84677072-2aa36680-af69-11ea-81bc-61cec12a238d.png">

----------------
2020-06-13(토): 맵 출력, 문 생성, 아이템 생성 완료
- 프로그램 실행 시, 초기에 맵이 정상적으로 안 뜨는 현상 해결(drawsBorder함수 추가)
- 수정할 사항: refresh 깜빡임 최적화 및 아이템 생성시 개수에 따라 가중치 달리 주기(ex: 1개-30%, 2개-50%, 3개-20%)

----------------
2020-06-14(일): 뱀 이동 대략적인 완성.

----------------
2020-06-15(월): 틱(0.1초) 구현, 스코어/미션 보드 관련 로직 및 출력 완성, 스테이지 중간벽 설정(Gate 생성 로직 추가)

<수정한 사항>
- 키를 연속으로 입력할 경우, 발생하던 화면 깜빡임 현상 해결(가장 바깥 윈도우 삭제)
- 아이템 생성 로직 일부 수정(증감 아이템 비율 2:1, 초기에는 증가 아이템만 생성)
- 게임 종료 or 스테이지 클리어 -> 알림 출력
- 게임 맵 상단에 현재 스테이지 출력

<추가할 사항>
- 기타 버그 수정(뱀이 벽 근처에서 방향을 틀 때, 꼬리부분이 벽을 건들면서 해당 벽좌표가 empty로 바뀌는 현상 등)
- getmaxyx 함수로 창 크기에 따른 윈도우 배치 고려
- 추가기능 구현
